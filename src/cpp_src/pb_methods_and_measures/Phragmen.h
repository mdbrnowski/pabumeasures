#include "utils/Election.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <optional>
#include <vector>

constexpr long double EPS = 1e-10;

bool is_less_than(long double a, long double b);

bool is_greater_than(long double a, long double b);

bool is_equal(long double a, long double b);

std::vector<ProjectEmbedding> phragmen(const Election &election, const ProjectComparator &tie_breaking);