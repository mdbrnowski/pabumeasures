#include "utils/Election.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <optional>
#include <vector>

std::vector<ProjectEmbedding> mes_cost(const Election &election, const ProjectComparator &tie_breaking);
