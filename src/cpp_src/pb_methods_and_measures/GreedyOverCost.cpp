#include "GreedyOverCost.h"
#include "utils/Election.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <algorithm>
#include <numeric>
#include <optional>
#include <vector>

long long ceil_div(long long a, long long b) { return (a + b - 1) / b; }

std::vector<ProjectEmbedding> greedy_over_cost(const Election &election, const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    auto projects = election.projects();
    std::vector<ProjectEmbedding> winners;
    std::sort(projects.begin(), projects.end(), [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        long long cross_term_a_approvals_b_cost = static_cast<long long>(a.approvers().size()) * b.cost(),
                  cross_term_b_approvals_a_cost = static_cast<long long>(b.approvers().size()) * a.cost();
        if (cross_term_a_approvals_b_cost == cross_term_b_approvals_a_cost) {
            return tie_breaking(a, b);
        }
        return cross_term_a_approvals_b_cost > cross_term_b_approvals_a_cost;
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

std::optional<int> optimist_add_for_greedy_over_cost(const Election &election, int p,
                                                     const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    int num_voters = election.numVoters();
    auto projects = election.projects();
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {}; // LCOV_EXCL_LINE (every project should be feasible)

    std::vector<ProjectEmbedding> winners;
    std::sort(projects.begin(), projects.end(), [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        long long cross_term_a_approvals_b_cost = static_cast<long long>(a.approvers().size()) * b.cost(),
                  cross_term_b_approvals_a_cost = static_cast<long long>(b.approvers().size()) * a.cost();
        if (cross_term_a_approvals_b_cost == cross_term_b_approvals_a_cost) {
            return tie_breaking(a, b);
        }
        return cross_term_a_approvals_b_cost > cross_term_b_approvals_a_cost;
    });
    for (const auto &project : projects) {
        if (project.cost() <= total_budget) {
            if (project == pp) {
                return 0;
            }
            if (pp.cost() <= total_budget && pp.cost() > total_budget - project.cost()) { // if (last moment to add pp)
                int new_approvers_size =
                    ceil_div(static_cast<long long>(project.approvers().size()) * pp.cost(), project.cost());
                std::vector<int> new_approvers(new_approvers_size);
                std::iota(new_approvers.begin(), new_approvers.end(), 0);
                auto new_pp = ProjectEmbedding(pp.cost(), pp.name(), new_approvers);
                if (static_cast<long long>(project.approvers().size()) * new_pp.cost() ==
                        static_cast<long long>(new_pp.approvers().size()) * project.cost() &&
                    tie_breaking(project, new_pp)) {
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
        if (total_budget <= 0)
            break;
    }
    return {}; // LCOV_EXCL_LINE (every project should be feasible)
}

std::optional<int> pessimist_add_for_greedy_over_cost(const Election &election, int p,
                                                      const ProjectComparator &tie_breaking) {
    return optimist_add_for_greedy_over_cost(election, p, tie_breaking);
}