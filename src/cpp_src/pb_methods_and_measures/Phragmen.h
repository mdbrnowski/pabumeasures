#include "utils/Election.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <optional>
#include <vector>

std::vector<ProjectEmbedding> phragmen(const Election &election, const ProjectComparator &tie_breaking);

std::optional<long long> cost_reduction_for_phragmen(const Election &election, int p,
                                                     const ProjectComparator &tie_breaking);
