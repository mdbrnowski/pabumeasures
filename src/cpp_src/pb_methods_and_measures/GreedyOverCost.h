#include "utils/Election.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <optional>
#include <vector>

std::vector<ProjectEmbedding> greedy_over_cost(const Election &election, const ProjectComparator &tie_breaking);

std::optional<long long> cost_reduction_for_greedy_over_cost(const Election &election, int p,
                                                             const ProjectComparator &tie_breaking);

std::optional<long long> optimist_add_for_greedy_over_cost(const Election &election, int p,
                                                           const ProjectComparator &tie_breaking);

std::optional<long long> pessimist_add_for_greedy_over_cost(const Election &election, int p,
                                                            const ProjectComparator &tie_breaking);

std::optional<long long> singleton_add_for_greedy_over_cost(const Election &election, int p,
                                                            const ProjectComparator &tie_breaking);
