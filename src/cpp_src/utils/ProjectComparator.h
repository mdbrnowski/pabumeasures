#pragma once
#include "ProjectEmbedding.h"
#include <compare>
#include <utility>
#include <vector>

class ProjectComparator {
  public:
    enum class Comparator { COST, VOTES, LEXICOGRAPHIC };
    enum class Ordering { ASCENDING, DESCENDING };

    explicit ProjectComparator(std::vector<std::pair<Comparator, Ordering>> criteria);
    ProjectComparator(Comparator comparator, Ordering ordering);

    bool operator()(const ProjectEmbedding &a, const ProjectEmbedding &b) const;

    // Static predefined comparators:
    static const ProjectComparator ByCostAsc;
    static const ProjectComparator ByCostDesc;
    static const ProjectComparator ByNameAsc;
    static const ProjectComparator ByNameDesc;
    static const ProjectComparator ByVotesDesc;
    static const ProjectComparator ByCostAscThenVotesDesc;
    static const ProjectComparator ByCostDescThenVotesDesc;

  private:
    std::vector<std::pair<Comparator, Ordering>> criteria_;

    static std::strong_ordering apply_order(std::strong_ordering cmp, Ordering order);
    static std::strong_ordering compare(const ProjectEmbedding &a, const ProjectEmbedding &b, Comparator cmp_type,
                                        Ordering order);
};
