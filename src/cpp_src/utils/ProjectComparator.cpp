#include "ProjectComparator.h"

ProjectComparator::ProjectComparator(std::vector<std::pair<Comparator, Ordering>> criteria)
    : criteria_(std::move(criteria)) {}

ProjectComparator::ProjectComparator(Comparator comparator, Ordering ordering)
    : criteria_{std::make_pair(comparator, ordering)} {}

bool ProjectComparator::operator()(const ProjectEmbedding &a, const ProjectEmbedding &b) const {
    for (const auto &[cmp_type, order] : criteria_) {
        auto cmp = compare(a, b, cmp_type, order);
        if (cmp != std::strong_ordering::equal) {
            return cmp == std::strong_ordering::less;
        }
    }
    // all equal - apply lexicographic ordering
    return compare(a, b, Comparator::LEXICOGRAPHIC, Ordering::ASCENDING) == std::strong_ordering::less;
    // todo: add information about tie-breaking ensuring total ordering to documentation
}

std::strong_ordering ProjectComparator::apply_order(std::strong_ordering cmp, Ordering order) {
    if (order == Ordering::ASCENDING)
        return cmp;
    if (cmp == std::strong_ordering::less)
        return std::strong_ordering::greater;
    if (cmp == std::strong_ordering::greater)
        return std::strong_ordering::less;
    return std::strong_ordering::equal;
}

std::strong_ordering ProjectComparator::compare(const ProjectEmbedding &a, const ProjectEmbedding &b,
                                                Comparator cmp_type, Ordering order) {
    switch (cmp_type) {
    case Comparator::COST:
        return apply_order(a.cost_ <=> b.cost_, order);
    case Comparator::VOTES:
        return apply_order(a.approvers_.size() <=> b.approvers_.size(), order);
    case Comparator::LEXICOGRAPHIC:
        return apply_order(a.name_ <=> b.name_, order);
    }
    return std::strong_ordering::equal; // LCOV_EXCL_LINE (project names should be different)
}

// Static predefined comparator definitions:
const ProjectComparator ProjectComparator::ByCostAsc{ProjectComparator::Comparator::COST,
                                                     ProjectComparator::Ordering::ASCENDING};
const ProjectComparator ProjectComparator::ByCostDesc{ProjectComparator::Comparator::COST,
                                                      ProjectComparator::Ordering::DESCENDING};
const ProjectComparator ProjectComparator::ByNameAsc{
    {}}; // Should be emptły <- Lexicographic sort is always the last step of comparison
const ProjectComparator ProjectComparator::ByNameDesc{ProjectComparator::Comparator::LEXICOGRAPHIC,
                                                      ProjectComparator::Ordering::DESCENDING};
const ProjectComparator ProjectComparator::ByVotesDesc{ProjectComparator::Comparator::VOTES,
                                                       ProjectComparator::Ordering::DESCENDING};
const ProjectComparator ProjectComparator::ByCostAscThenVotesDesc{
    std::vector<std::pair<ProjectComparator::Comparator, ProjectComparator::Ordering>>{
        {ProjectComparator::Comparator::COST, ProjectComparator::Ordering::ASCENDING},
        {ProjectComparator::Comparator::VOTES, ProjectComparator::Ordering::DESCENDING}}};
const ProjectComparator ProjectComparator::ByCostDescThenVotesDesc{
    std::vector<std::pair<ProjectComparator::Comparator, ProjectComparator::Ordering>>{
        {ProjectComparator::Comparator::COST, ProjectComparator::Ordering::DESCENDING},
        {ProjectComparator::Comparator::VOTES, ProjectComparator::Ordering::DESCENDING}}};
