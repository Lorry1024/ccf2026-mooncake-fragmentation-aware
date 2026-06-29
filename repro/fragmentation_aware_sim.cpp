#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr std::uint64_t MiB = 1024ULL * 1024ULL;

struct FreeBlock {
    std::uint64_t size;
};

struct Segment {
    std::string name;
    std::uint64_t capacity;
    std::uint64_t used;
    std::vector<FreeBlock> free_blocks;
};

std::uint64_t totalFree(const Segment& segment) {
    std::uint64_t total = 0;
    for (const auto& block : segment.free_blocks) {
        total += block.size;
    }
    return total;
}

std::uint64_t largestFreeRegion(const Segment& segment) {
    std::uint64_t largest = 0;
    for (const auto& block : segment.free_blocks) {
        largest = std::max(largest, block.size);
    }
    return largest;
}

double freeRatio(const Segment& segment) {
    return static_cast<double>(totalFree(segment)) /
           static_cast<double>(segment.capacity);
}

double fragmentationAwareScore(const Segment& segment) {
    const auto free = totalFree(segment);
    if (segment.capacity == 0 || free == 0) return 0.0;
    const double free_ratio =
        static_cast<double>(free) / static_cast<double>(segment.capacity);
    const double contiguity_ratio =
        static_cast<double>(largestFreeRegion(segment)) /
        static_cast<double>(free);
    return 0.70 * contiguity_ratio + 0.30 * free_ratio;
}

const Segment& chooseFreeRatioFirst(const std::vector<Segment>& segments) {
    return *std::max_element(
        segments.begin(), segments.end(), [](const Segment& lhs,
                                             const Segment& rhs) {
            return freeRatio(lhs) < freeRatio(rhs);
        });
}

const Segment& chooseFragmentationAware(const std::vector<Segment>& segments,
                                        std::uint64_t request_size) {
    return *std::max_element(
        segments.begin(), segments.end(), [request_size](const Segment& lhs,
                                                        const Segment& rhs) {
            const bool lhs_can_fit = largestFreeRegion(lhs) >= request_size;
            const bool rhs_can_fit = largestFreeRegion(rhs) >= request_size;
            if (lhs_can_fit != rhs_can_fit) return !lhs_can_fit;

            const double lhs_score = fragmentationAwareScore(lhs);
            const double rhs_score = fragmentationAwareScore(rhs);
            if (lhs_score != rhs_score) return lhs_score < rhs_score;

            return largestFreeRegion(lhs) < largestFreeRegion(rhs);
        });
}

void printSegment(const Segment& segment, std::uint64_t request_size) {
    std::cout << std::left << std::setw(12) << segment.name << std::right
              << std::setw(10) << totalFree(segment) / MiB << std::setw(12)
              << largestFreeRegion(segment) / MiB << std::setw(12)
              << std::fixed << std::setprecision(3) << freeRatio(segment)
              << std::setw(12) << fragmentationAwareScore(segment)
              << std::setw(10)
              << (largestFreeRegion(segment) >= request_size ? "yes" : "no")
              << '\n';
}

}  // namespace

int main() {
    const std::uint64_t request_size = 10 * MiB;
    const std::vector<Segment> segments = {
        {
            "fragmented",
            32 * MiB,
            16 * MiB,
            {{8 * MiB}, {8 * MiB}},
        },
        {
            "contiguous",
            32 * MiB,
            20 * MiB,
            {{12 * MiB}},
        },
    };

    std::cout << "request_mib=" << request_size / MiB << "\n\n";
    std::cout << std::left << std::setw(12) << "segment" << std::right
              << std::setw(10) << "free_mib" << std::setw(12)
              << "largest_mib" << std::setw(12) << "free_ratio"
              << std::setw(12) << "fa_score" << std::setw(10) << "can_fit"
              << '\n';
    for (const auto& segment : segments) {
        printSegment(segment, request_size);
    }

    const auto& free_ratio_choice = chooseFreeRatioFirst(segments);
    const auto& fragmentation_choice =
        chooseFragmentationAware(segments, request_size);

    std::cout << "\nfree_ratio_first_choice=" << free_ratio_choice.name
              << " can_fit="
              << (largestFreeRegion(free_ratio_choice) >= request_size ? "yes"
                                                                        : "no")
              << '\n';
    std::cout << "fragmentation_aware_choice=" << fragmentation_choice.name
              << " can_fit="
              << (largestFreeRegion(fragmentation_choice) >= request_size
                      ? "yes"
                      : "no")
              << '\n';

    return fragmentation_choice.name == "contiguous" &&
                   largestFreeRegion(fragmentation_choice) >= request_size
               ? 0
               : 1;
}
