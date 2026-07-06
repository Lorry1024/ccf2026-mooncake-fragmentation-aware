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
    std::vector<FreeBlock> free_blocks;
};

struct Scenario {
    std::string name;
    std::uint64_t request_size;
    std::vector<Segment> segments;
    std::string expected_free_ratio_choice;
    bool expected_free_ratio_can_fit;
    std::string expected_fragmentation_choice;
    bool expected_fragmentation_can_fit;
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
    const auto largest = largestFreeRegion(segment);
    return std::min(1.0, static_cast<double>(largest) /
                             static_cast<double>(free));
}

double fragmentationAwareScore(const Segment& segment) {
    return 0.70 * contiguityRatio(segment) + 0.30 * freeRatio(segment);
}

bool canFit(const Segment& segment, std::uint64_t request_size) {
    return largestFreeRegion(segment) >= request_size;
}

const Segment& chooseFreeRatioFirst(const std::vector<Segment>& segments) {
    return *std::max_element(
        segments.begin(), segments.end(), [](const Segment& lhs,
                                             const Segment& rhs) {
            if (freeRatio(lhs) != freeRatio(rhs)) {
                return freeRatio(lhs) < freeRatio(rhs);
            }
            return largestFreeRegion(lhs) < largestFreeRegion(rhs);
        });
}

const Segment& chooseFragmentationAware(const std::vector<Segment>& segments,
                                        std::uint64_t request_size) {
    return *std::max_element(
        segments.begin(), segments.end(), [request_size](const Segment& lhs,
                                                        const Segment& rhs) {
            const bool lhs_can_fit = canFit(lhs, request_size);
            const bool rhs_can_fit = canFit(rhs, request_size);
            if (lhs_can_fit != rhs_can_fit) {
                return !lhs_can_fit;
            }

            const double lhs_score = fragmentationAwareScore(lhs);
            const double rhs_score = fragmentationAwareScore(rhs);
            if (lhs_score != rhs_score) {
                return lhs_score < rhs_score;
            }

            return largestFreeRegion(lhs) < largestFreeRegion(rhs);
        });
}

void printCsvHeader() {
    std::cout
        << "scenario,request_mib,segment,capacity_mib,free_mib,largest_mib,"
           "free_ratio,contiguity_ratio,fa_score,can_fit,"
           "free_ratio_first_choice,free_ratio_first_can_fit,"
           "fragmentation_aware_choice,fragmentation_aware_can_fit\n";
}

bool runScenario(const Scenario& scenario) {
    const auto& free_ratio_choice = chooseFreeRatioFirst(scenario.segments);
    const auto& fragmentation_choice =
        chooseFragmentationAware(scenario.segments, scenario.request_size);

    const bool free_ratio_can_fit =
        canFit(free_ratio_choice, scenario.request_size);
    const bool fragmentation_can_fit =
        canFit(fragmentation_choice, scenario.request_size);

    for (const auto& segment : scenario.segments) {
        std::cout << scenario.name << ',' << scenario.request_size / MiB << ','
                  << segment.name << ',' << segment.capacity / MiB << ','
                  << totalFree(segment) / MiB << ','
                  << largestFreeRegion(segment) / MiB << ',' << std::fixed
                  << std::setprecision(3) << freeRatio(segment) << ','
                  << contiguityRatio(segment) << ','
                  << fragmentationAwareScore(segment) << ','
                  << (canFit(segment, scenario.request_size) ? "yes" : "no")
                  << ',' << free_ratio_choice.name << ','
                  << (free_ratio_can_fit ? "yes" : "no") << ','
                  << fragmentation_choice.name << ','
                  << (fragmentation_can_fit ? "yes" : "no") << '\n';
    }

    return free_ratio_choice.name == scenario.expected_free_ratio_choice &&
           free_ratio_can_fit == scenario.expected_free_ratio_can_fit &&
           fragmentation_choice.name ==
               scenario.expected_fragmentation_choice &&
           fragmentation_can_fit == scenario.expected_fragmentation_can_fit;
}

}  // namespace

int main() {
    const std::vector<Scenario> scenarios = {
        {
            "mixed_10m",
            10 * MiB,
            {
                {"fragmented", 32 * MiB, {{8 * MiB}, {8 * MiB}}},
                {"contiguous", 32 * MiB, {{12 * MiB}}},
            },
            "fragmented",
            false,
            "contiguous",
            true,
        },
        {
            "boundary_8m",
            8 * MiB,
            {
                {"fragmented", 32 * MiB, {{8 * MiB}, {8 * MiB}}},
                {"contiguous", 32 * MiB, {{12 * MiB}}},
            },
            "fragmented",
            true,
            "contiguous",
            true,
        },
        {
            "boundary_12m",
            12 * MiB,
            {
                {"fragmented", 32 * MiB, {{8 * MiB}, {8 * MiB}}},
                {"contiguous", 32 * MiB, {{12 * MiB}}},
            },
            "fragmented",
            false,
            "contiguous",
            true,
        },
        {
            "no_fit_20m",
            20 * MiB,
            {
                {"fragmented", 32 * MiB, {{8 * MiB}, {8 * MiB}}},
                {"contiguous", 32 * MiB, {{12 * MiB}}},
            },
            "fragmented",
            false,
            "contiguous",
            false,
        },
        {
            "zero_free_guard",
            1 * MiB,
            {
                {"empty", 32 * MiB, {}},
                {"single_hole", 32 * MiB, {{2 * MiB}}},
            },
            "single_hole",
            true,
            "single_hole",
            true,
        },
    };

    printCsvHeader();
    bool ok = true;
    for (const auto& scenario : scenarios) {
        ok = runScenario(scenario) && ok;
    }

    std::cerr << (ok ? "PASS" : "FAIL")
              << " fragmentation_aware_metrics: validated "
              << scenarios.size()
              << " deterministic ranking and boundary scenarios\n";
    return ok ? 0 : 1;
}
