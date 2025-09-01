#include "Greedy.h"
#include "utils/Election.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <algorithm>
#include <numeric>
#include <optional>
#include <vector>

std::vector<ProjectEmbedding> greedy(const Election &election, const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    auto projects = election.projects();
    std::vector<ProjectEmbedding> winners;
    std::ranges::sort(projects, [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        if (a.approvers().size() == b.approvers().size()) {
            return tie_breaking(a, b);
        }
        return a.approvers().size() > b.approvers().size();
    });
    for (const auto &project : projects) {
        if (project.cost() <= total_budget) {
            winners.push_back(project);
            total_budget -= project.cost();
        }
        if (total_budget <= 0)
            break;
    }
    return winners;
}

std::optional<int> cost_reduction_for_greedy(const Election &election, int p, const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    int num_voters = election.numVoters();
    auto projects = election.projects();
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {}; // LCOV_EXCL_LINE (every project should be feasible)

    std::optional<int> max_price_to_be_chosen{};

    std::vector<ProjectEmbedding> winners;
    std::ranges::sort(projects, [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        if (a.approvers().size() == b.approvers().size()) {
            return tie_breaking(a, b);
        }
        return a.approvers().size() > b.approvers().size();
    });

    for (const auto &project : projects) {
        if (project.approvers().size() < pp.approvers().size()) {
            break;
        }
        if (project.cost() <= total_budget) {
            if (project == pp) {
                return pp.cost();
            }
            if (project.approvers().size() == pp.approvers().size() &&
                tie_breaking(ProjectEmbedding(project.cost() - 1, pp.name(), pp.approvers()),
                             project)) { // Not taken because lost tie-breaking
                if (max_price_to_be_chosen) {
                    *max_price_to_be_chosen = std::max(*max_price_to_be_chosen, static_cast<int>(project.cost() - 1));
                } else {
                    max_price_to_be_chosen = static_cast<int>(project.cost() - 1);
                }
            }
            winners.push_back(project);
            total_budget -= project.cost();
        } else if (project == pp) { // not taken because budget too tight
            if (max_price_to_be_chosen) {
                *max_price_to_be_chosen = std::max(*max_price_to_be_chosen, static_cast<int>(total_budget));
            } else {
                max_price_to_be_chosen = static_cast<int>(total_budget);
            }
        }
    }
    return max_price_to_be_chosen;
}

std::optional<int> optimist_add_for_greedy(const Election &election, int p, const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    int num_voters = election.numVoters();
    auto projects = election.projects();
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {}; // LCOV_EXCL_LINE (every project should be feasible)

    std::vector<ProjectEmbedding> winners;
    std::ranges::sort(projects, [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        if (a.approvers().size() == b.approvers().size()) {
            return tie_breaking(a, b);
        }
        return a.approvers().size() > b.approvers().size();
    });
    for (const auto &project : projects) {
        if (project.cost() <= total_budget) {
            if (project == pp) {
                return 0;
            }
            if (pp.cost() > total_budget - project.cost()) { // if (last moment to add pp)
                int new_approvers_size = project.approvers().size();
                std::vector<int> new_approvers(new_approvers_size);
                std::iota(new_approvers.begin(), new_approvers.end(), 0);
                auto new_pp = ProjectEmbedding(pp.cost(), pp.name(), new_approvers);
                if (tie_breaking(project, new_pp)) {
                    new_approvers_size += 1;
                }
                if (new_approvers_size > num_voters)
                    return {};
                else
                    return new_approvers_size - pp.approvers().size();
            }
            winners.push_back(project);
            total_budget -= project.cost();
        }
    }
    return {}; // LCOV_EXCL_LINE (every project should be feasible)
}

std::optional<int> pessimist_add_for_greedy(const Election &election, int p, const ProjectComparator &tie_breaking) {
    return optimist_add_for_greedy(election, p, tie_breaking);
}

std::optional<int> singleton_add_for_greedy(const Election &election, int p, const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    auto projects = election.projects();
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {}; // LCOV_EXCL_LINE (every project should be feasible)

    std::vector<ProjectEmbedding> winners;
    std::ranges::sort(projects, [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        if (a.approvers().size() == b.approvers().size()) {
            return tie_breaking(a, b);
        }
        return a.approvers().size() > b.approvers().size();
    });
    for (const auto &project : projects) {
        if (project.cost() <= total_budget) {
            if (project == pp) {
                return 0;
            }
            if (pp.cost() > total_budget - project.cost()) { // if (last moment to add pp)
                int new_approvers_size = project.approvers().size();
                std::vector<int> new_approvers(new_approvers_size);
                std::iota(new_approvers.begin(), new_approvers.end(), 0);
                auto new_pp = ProjectEmbedding(pp.cost(), pp.name(), new_approvers);
                if (tie_breaking(project, new_pp)) {
                    new_approvers_size += 1;
                }
                return new_approvers_size - pp.approvers().size();
            }
            winners.push_back(project);
            total_budget -= project.cost();
        }
    }
    return {}; // LCOV_EXCL_LINE (every project should be feasible)
}
