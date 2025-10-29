#include "utils/Election.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <optional>
#include <vector>

std::vector<ProjectEmbedding> phragmen(const Election &election, const ProjectComparator &tie_breaking);

long long cost_reduction_for_phragmen(const Election &election, int p, const ProjectComparator &tie_breaking);

std::optional<int> optimist_add_for_phragmen(const Election &election, int p, const ProjectComparator &tie_breaking);

std::optional<int> pessimist_add_for_phragmen(const Election &election, int p, const ProjectComparator &tie_breaking);

std::optional<int> singleton_add_for_phragmen(const Election &election, int p, const ProjectComparator &tie_breaking);
