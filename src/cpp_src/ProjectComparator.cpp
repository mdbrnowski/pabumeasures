#include "ProjectComparator.h"

ProjectComparator::ProjectComparator(std::vector<std::pair<Comparator, Ordering>> criteria)
    : criteria_(std::move(criteria)) {}

ProjectComparator::ProjectComparator(Comparator comparator, Ordering ordering)
    : criteria_{std::make_pair(comparator, ordering)} {}

bool ProjectComparator::operator()(const Project &a, const Project &b) const {
    for (const auto &[cmpType, order] : criteria_) {
        auto cmp = compare(a, b, cmpType, order);
        if (cmp != std::strong_ordering::equal) {
            return cmp == std::strong_ordering::less;
        }
    }
    return false; // all equal
}

std::strong_ordering ProjectComparator::applyOrder(std::strong_ordering cmp, Ordering order) {
    if (order == Ordering::ASCENDING)
        return cmp;
    if (cmp == std::strong_ordering::less)
        return std::strong_ordering::greater;
    if (cmp == std::strong_ordering::greater)
        return std::strong_ordering::less;
    return std::strong_ordering::equal;
}

std::strong_ordering ProjectComparator::compare(const Project &a, const Project &b, Comparator cmpType,
                                                Ordering order) {
    switch (cmpType) {
    case Comparator::COST:
        return applyOrder(a.cost_ <=> b.cost_, order);
    case Comparator::VOTES:
        return applyOrder(a.approvers_.size() <=> b.approvers_.size(), order);
    case Comparator::LEXICOGRAPHIC:
        return applyOrder(a.name_ <=> b.name_, order);
    }
    return std::strong_ordering::equal;
}

// Static predefined comparator definitions:
const ProjectComparator ProjectComparator::ByCostAsc{ProjectComparator::Comparator::COST,
                                                     ProjectComparator::Ordering::ASCENDING};
const ProjectComparator ProjectComparator::ByVotesDesc{ProjectComparator::Comparator::VOTES,
                                                       ProjectComparator::Ordering::DESCENDING};
const ProjectComparator ProjectComparator::ByCostAscThenVotesDesc{
    std::vector<std::pair<ProjectComparator::Comparator, ProjectComparator::Ordering>>{
        {ProjectComparator::Comparator::COST, ProjectComparator::Ordering::ASCENDING},
        {ProjectComparator::Comparator::VOTES, ProjectComparator::Ordering::DESCENDING}}};