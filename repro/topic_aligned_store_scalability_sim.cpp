#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace {

constexpr std::uint64_t MiB = 1024ULL * 1024ULL;

struct Segment {
    std::string name;
    std::uint64_t capacity;
    std::vector<std::uint64_t> free_blocks;
};

struct Decision {
    std::string choice;
    bool primary_can_fit = false;
    bool eventual_can_fit = false;
    int attempts_to_fit = 0;
    int fallback_attempts = 0;
    int candidates_scored = 0;
};

std::uint64_t totalFree(const Segment& segment) {
    return std::accumulate(segment.free_blocks.begin(), segment.free_blocks.end(),
                           std::uint64_t{0});
}

std::uint64_t largestFree(const Segment& segment) {
    if (segment.free_blocks.empty()) {
        return 0;
    }
    return *std::max_element(segment.free_blocks.begin(), segment.free_blocks.end());
}

double freeRatio(const Segment& segment) {
    if (segment.capacity == 0) {
        return 0.0;
    }
    return static_cast<double>(totalFree(segment)) /
           static_cast<double>(segment.capacity);
}

double contiguityRatio(const Segment& segment) {
    const auto free = totalFree(segment);
    if (free == 0) {
        return 0.0;
    }
    return std::min(1.0, static_cast<double>(largestFree(segment)) /
                             static_cast<double>(free));
}

double faScore(const Segment& segment) {
    return 0.70 * contiguityRatio(segment) + 0.30 * freeRatio(segment);
}

bool canFit(const Segment& segment, std::uint64_t request_size) {
    return largestFree(segment) >= request_size;
}

Decision decideFreeRatioFirst(const std::vector<Segment>& segments,
                              std::uint64_t request_size) {
    std::vector<const Segment*> ranked;
    ranked.reserve(segments.size());
    for (const auto& segment : segments) {
        ranked.push_back(&segment);
    }

    std::sort(ranked.begin(), ranked.end(), [](const Segment* lhs, const Segment* rhs) {
        if (freeRatio(*lhs) != freeRatio(*rhs)) {
            return freeRatio(*lhs) > freeRatio(*rhs);
        }
        return largestFree(*lhs) > largestFree(*rhs);
    });

    Decision decision;
    decision.choice = ranked.front()->name;
    decision.primary_can_fit = canFit(*ranked.front(), request_size);
    decision.candidates_scored = static_cast<int>(ranked.size());

    for (const auto* segment : ranked) {
        ++decision.attempts_to_fit;
        if (canFit(*segment, request_size)) {
            decision.eventual_can_fit = true;
            break;
        }
        ++decision.fallback_attempts;
    }
    return decision;
}

Decision decideFragmentationAware(const std::vector<Segment>& segments,
                                  std::uint64_t request_size) {
    std::vector<const Segment*> ranked;
    ranked.reserve(segments.size());
    for (const auto& segment : segments) {
        ranked.push_back(&segment);
    }

    std::sort(ranked.begin(), ranked.end(),
              [request_size](const Segment* lhs, const Segment* rhs) {
                  const bool lhs_fit = canFit(*lhs, request_size);
                  const bool rhs_fit = canFit(*rhs, request_size);
                  if (lhs_fit != rhs_fit) {
                      return lhs_fit;
                  }
                  if (faScore(*lhs) != faScore(*rhs)) {
                      return faScore(*lhs) > faScore(*rhs);
                  }
                  if (largestFree(*lhs) != largestFree(*rhs)) {
                      return largestFree(*lhs) > largestFree(*rhs);
                  }
                  return freeRatio(*lhs) > freeRatio(*rhs);
              });

    Decision decision;
    decision.choice = ranked.front()->name;
    decision.primary_can_fit = canFit(*ranked.front(), request_size);
    decision.candidates_scored = static_cast<int>(ranked.size());

    for (const auto* segment : ranked) {
        ++decision.attempts_to_fit;
        if (canFit(*segment, request_size)) {
            decision.eventual_can_fit = true;
            break;
        }
        ++decision.fallback_attempts;
    }
    return decision;
}

template <typename Fn>
double nsPerDecision(Fn fn, const std::vector<Segment>& segments,
                     const std::vector<std::uint64_t>& requests,
                     int iterations) {
    volatile int sink = 0;
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        for (const auto request : requests) {
            const auto decision = fn(segments, request);
            sink += decision.primary_can_fit ? 1 : 0;
            sink += decision.candidates_scored;
        }
    }
    const auto end = std::chrono::steady_clock::now();
    const auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return static_cast<double>(ns) /
           static_cast<double>(iterations * requests.size());
}

}  // namespace

int main() {
    const std::vector<Segment> segments = {
        {"fragmented_high_free_a", 64 * MiB,
         {4 * MiB, 4 * MiB, 4 * MiB, 4 * MiB, 4 * MiB, 4 * MiB, 4 * MiB,
          4 * MiB}},
        {"fragmented_high_free_b", 64 * MiB,
         {6 * MiB, 6 * MiB, 6 * MiB, 6 * MiB, 6 * MiB}},
        {"contiguous_large_a", 64 * MiB, {24 * MiB}},
        {"contiguous_large_b", 64 * MiB, {16 * MiB}},
        {"small_holes", 64 * MiB, {8 * MiB, 8 * MiB}},
    };

    const std::vector<std::uint64_t> requests = {
        20 * MiB, 18 * MiB, 12 * MiB, 10 * MiB, 8 * MiB, 6 * MiB};

    int free_ratio_primary_success = 0;
    int fa_primary_success = 0;
    int free_ratio_eventual_success = 0;
    int fa_eventual_success = 0;
    int free_ratio_fallback_attempts = 0;
    int fa_fallback_attempts = 0;
    int free_ratio_candidates = 0;
    int fa_candidates = 0;

    std::cout
        << "request_mib,free_ratio_choice,free_ratio_primary_can_fit,"
           "free_ratio_attempts_to_fit,free_ratio_fallback_attempts,"
           "fragmentation_aware_choice,fragmentation_aware_primary_can_fit,"
           "fragmentation_aware_attempts_to_fit,"
           "fragmentation_aware_fallback_attempts,candidates_scored\n";

    for (const auto request : requests) {
        const auto free_ratio = decideFreeRatioFirst(segments, request);
        const auto fa = decideFragmentationAware(segments, request);

        free_ratio_primary_success += free_ratio.primary_can_fit ? 1 : 0;
        fa_primary_success += fa.primary_can_fit ? 1 : 0;
        free_ratio_eventual_success += free_ratio.eventual_can_fit ? 1 : 0;
        fa_eventual_success += fa.eventual_can_fit ? 1 : 0;
        free_ratio_fallback_attempts += free_ratio.fallback_attempts;
        fa_fallback_attempts += fa.fallback_attempts;
        free_ratio_candidates += free_ratio.candidates_scored;
        fa_candidates += fa.candidates_scored;

        std::cout << request / MiB << ',' << free_ratio.choice << ','
                  << (free_ratio.primary_can_fit ? "yes" : "no") << ','
                  << free_ratio.attempts_to_fit << ','
                  << free_ratio.fallback_attempts << ',' << fa.choice << ','
                  << (fa.primary_can_fit ? "yes" : "no") << ','
                  << fa.attempts_to_fit << ',' << fa.fallback_attempts << ','
                  << fa.candidates_scored << '\n';
    }

    const int requests_count = static_cast<int>(requests.size());
    const int iterations = 200000;
    const double free_ratio_ns = nsPerDecision(decideFreeRatioFirst, segments,
                                               requests, iterations);
    const double fa_ns =
        nsPerDecision(decideFragmentationAware, segments, requests, iterations);

    std::cerr << std::fixed << std::setprecision(2);
    std::cerr << "SUMMARY topic_aligned_store_scalability_sim\n";
    std::cerr << "requests=" << requests_count << '\n';
    std::cerr << "free_ratio_primary_success=" << free_ratio_primary_success
              << "/" << requests_count << '\n';
    std::cerr << "fragmentation_aware_primary_success=" << fa_primary_success
              << "/" << requests_count << '\n';
    std::cerr << "free_ratio_eventual_success=" << free_ratio_eventual_success
              << "/" << requests_count << '\n';
    std::cerr << "fragmentation_aware_eventual_success=" << fa_eventual_success
              << "/" << requests_count << '\n';
    std::cerr << "free_ratio_fallback_attempts="
              << free_ratio_fallback_attempts << '\n';
    std::cerr << "fragmentation_aware_fallback_attempts=" << fa_fallback_attempts
              << '\n';
    std::cerr << "avg_candidates_scored_free_ratio="
              << static_cast<double>(free_ratio_candidates) / requests_count
              << '\n';
    std::cerr << "avg_candidates_scored_fragmentation_aware="
              << static_cast<double>(fa_candidates) / requests_count << '\n';
    std::cerr << "free_ratio_decision_ns_per_request=" << free_ratio_ns << '\n';
    std::cerr << "fragmentation_aware_decision_ns_per_request=" << fa_ns
              << '\n';
    std::cerr << "fragmentation_aware_extra_ns_per_request="
              << (fa_ns - free_ratio_ns) << '\n';
    std::cerr << "NOTE: deterministic local simulation only; not a real "
                 "Mooncake/SGLang HiCache benchmark.\n";

    const bool ok = fa_primary_success > free_ratio_primary_success &&
                    fa_fallback_attempts < free_ratio_fallback_attempts &&
                    fa_candidates == free_ratio_candidates;
    std::cerr << (ok ? "PASS" : "FAIL")
              << " topic_aligned_store_scalability_sim\n";
    return ok ? 0 : 1;
}
