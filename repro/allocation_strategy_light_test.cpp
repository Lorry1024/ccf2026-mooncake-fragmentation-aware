#include "allocation_strategy.h"

#include <glog/logging.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace mooncake {

MasterMetricManager& MasterMetricManager::instance() {
    static std::aligned_storage_t<sizeof(MasterMetricManager),
                                  alignof(MasterMetricManager)>
        storage;
    return *reinterpret_cast<MasterMetricManager*>(&storage);
}

void MasterMetricManager::inc_allocated_file_size(int64_t) {}
void MasterMetricManager::dec_allocated_file_size(int64_t) {}

namespace offset_allocator {

OffsetAllocationHandle::OffsetAllocationHandle(
    OffsetAllocationHandle&& other) noexcept
    : m_allocator(std::move(other.m_allocator)),
      m_allocation(other.m_allocation),
      real_base(std::exchange(other.real_base, 0)),
      requested_size(std::exchange(other.requested_size, 0)) {}

OffsetAllocationHandle& OffsetAllocationHandle::operator=(
    OffsetAllocationHandle&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    m_allocator = std::move(other.m_allocator);
    m_allocation = other.m_allocation;
    real_base = std::exchange(other.real_base, 0);
    requested_size = std::exchange(other.requested_size, 0);
    return *this;
}

OffsetAllocationHandle::~OffsetAllocationHandle() = default;

}  // namespace offset_allocator

AllocatedBuffer::~AllocatedBuffer() {
    auto alloc = allocator_.lock();
    if (alloc) {
        alloc->deallocate(this);
    }
}

AllocatedBuffer::AllocatedBuffer(AllocatedBuffer&& other) noexcept
    : allocator_(std::move(other.allocator_)),
      segment_name_(std::move(other.segment_name_)),
      buffer_ptr_(std::exchange(other.buffer_ptr_, nullptr)),
      size_(std::exchange(other.size_, 0)),
      protocol(std::move(other.protocol)),
      offset_handle_(std::move(other.offset_handle_)) {}

AllocatedBuffer& AllocatedBuffer::operator=(AllocatedBuffer&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    auto alloc = allocator_.lock();
    if (alloc && buffer_ptr_ != nullptr) {
        alloc->deallocate(this);
    }
    allocator_ = std::move(other.allocator_);
    segment_name_ = std::move(other.segment_name_);
    buffer_ptr_ = std::exchange(other.buffer_ptr_, nullptr);
    size_ = std::exchange(other.size_, 0);
    protocol = std::move(other.protocol);
    offset_handle_ = std::move(other.offset_handle_);
    return *this;
}

AllocatedBuffer::Descriptor AllocatedBuffer::get_descriptor() const {
    auto alloc = allocator_.lock();
    return {static_cast<uint64_t>(size_),
            reinterpret_cast<uintptr_t>(buffer_ptr_), protocol,
            alloc ? alloc->getTransportEndpoint() : std::string{}};
}

std::string AllocatedBuffer::getSegmentName() const noexcept {
    auto alloc = allocator_.lock();
    return alloc ? alloc->getSegmentName() : std::string{};
}

void AllocatedBuffer::change_to_cxl(std::string client_segment_name) {
    segment_name_ = std::move(client_segment_name);
    protocol = "cxl";
}

void* AllocatedBuffer::get_vaddr_from_cxl() { return buffer_ptr_; }

class FakeAllocator : public BufferAllocatorBase,
                      public std::enable_shared_from_this<FakeAllocator> {
   public:
    FakeAllocator(std::string name, size_t capacity, size_t used,
                  size_t largest_free_region)
        : name_(std::move(name)),
          capacity_(capacity),
          used_(used),
          largest_free_region_(largest_free_region),
          base_(next_base_) {
        next_base_ += capacity + kAddressGap;
    }

    std::unique_ptr<AllocatedBuffer> allocate(size_t size) override {
        ++attempts_;
        const size_t free = capacity_ - std::min(used_, capacity_);
        if (size > free || size > largest_free_region_) {
            return nullptr;
        }
        used_ += size;
        largest_free_region_ =
            largest_free_region_ > size ? largest_free_region_ - size : 0;
        return std::make_unique<AllocatedBuffer>(
            shared_from_this(), reinterpret_cast<void*>(base_ + used_), size);
    }

    void deallocate(AllocatedBuffer* handle) override {
        if (!handle) {
            return;
        }
        const size_t freed = std::min(handle->size(), used_);
        used_ -= freed;
        largest_free_region_ += freed;
    }

    size_t capacity() const override { return capacity_; }
    size_t size() const override { return used_; }
    std::string getSegmentName() const override { return name_; }
    std::string getTransportEndpoint() const override { return name_; }
    size_t getLargestFreeRegion() const override {
        return largest_free_region_;
    }
    int attempts() const { return attempts_; }

   private:
    static constexpr uintptr_t kAddressGap = 1ULL << 28;
    inline static uintptr_t next_base_ = 0x100000000ULL;

    std::string name_;
    size_t capacity_;
    size_t used_;
    size_t largest_free_region_;
    uintptr_t base_;
    int attempts_ = 0;
};

}  // namespace mooncake

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);

    using namespace mooncake;
    constexpr size_t MiB = 1024ULL * 1024;

    {
        AllocatorManager manager;
        auto fragmented =
            std::make_shared<FakeAllocator>("fragmented", 32 * MiB, 16 * MiB,
                                            8 * MiB);
        auto contiguous =
            std::make_shared<FakeAllocator>("contiguous", 32 * MiB, 20 * MiB,
                                            12 * MiB);
        manager.addAllocator("fragmented", fragmented);
        manager.addAllocator("contiguous", contiguous);

        FragmentationAwareAllocationStrategy strategy;
        auto result = strategy.Allocate(manager, 10 * MiB, 1, {}, {});
        assert(result.has_value());
        assert(result->size() == 1);
        auto desc = result->front().get_descriptor();
        assert(desc.get_memory_descriptor()
                   .buffer_descriptor.transport_endpoint_ == "contiguous");
        assert(fragmented->attempts() == 0);
        assert(contiguous->attempts() == 1);
    }

    {
        AllocatorManager manager;
        auto fragmented =
            std::make_shared<FakeAllocator>("fragmented", 32 * MiB, 16 * MiB,
                                            8 * MiB);
        auto contiguous =
            std::make_shared<FakeAllocator>("contiguous", 32 * MiB, 20 * MiB,
                                            12 * MiB);
        manager.addAllocator("fragmented", fragmented);
        manager.addAllocator("contiguous", contiguous);

        FreeRatioFirstAllocationStrategy strategy;
        auto result = strategy.Allocate(manager, 10 * MiB, 1, {}, {});
        assert(result.has_value());
        assert(result->size() == 1);
        assert(fragmented->attempts() >= 1);
        assert(contiguous->attempts() >= 1);
    }

    {
        AllocatorManager manager;
        auto fragmented =
            std::make_shared<FakeAllocator>("fragmented", 32 * MiB, 16 * MiB,
                                            8 * MiB);
        auto contiguous =
            std::make_shared<FakeAllocator>("contiguous", 32 * MiB, 20 * MiB,
                                            12 * MiB);
        manager.addAllocator("fragmented", fragmented);
        manager.addAllocator("contiguous", contiguous);

        FragmentationAwareAllocationStrategy strategy;
        auto result =
            strategy.Allocate(manager, 4 * MiB, 1, {"fragmented"}, {});
        assert(result.has_value());
        auto desc = result->front().get_descriptor();
        assert(desc.get_memory_descriptor()
                   .buffer_descriptor.transport_endpoint_ == "fragmented");
    }

    std::cout << "PASS allocation_strategy_light_test: fragmentation-aware "
                 "chooses contiguous-fit candidates and preserves preferred "
                 "segments\n";
    return 0;
}
