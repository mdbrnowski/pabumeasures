#include "GreedyOverCost.h"
#include "utils/Election.h"
#include "utils/Math.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <algorithm>
#include <numeric>
#include <optional>
#include <vector>

std::vector<ProjectEmbedding> greedy_over_cost(const Election &election, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto projects = election.projects();
    std::vector<ProjectEmbedding> winners;
    std::ranges::sort(projects, [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        long long cross_term_a_approvals_b_cost = a.num_of_approvers() * b.cost(),
                  cross_term_b_approvals_a_cost = b.num_of_approvers() * a.cost();
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

long long cost_reduction_for_greedy_over_cost(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto projects = election.projects();
    auto pp = projects[p];

    long long max_price_to_be_chosen = 0;

    std::ranges::sort(projects, [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        long long cross_term_a_approvals_b_cost = a.num_of_approvers() * b.cost(),
                  cross_term_b_approvals_a_cost = b.num_of_approvers() * a.cost();
        if (cross_term_a_approvals_b_cost == cross_term_b_approvals_a_cost) {
            return tie_breaking(a, b);
        }
        return cross_term_a_approvals_b_cost > cross_term_b_approvals_a_cost;
    });
    for (const auto &project : projects) {
        if (project.cost() <= total_budget) {
            if (project == pp) {
                return pp.cost();
            } else {
                long long curr_max_price = 0;
                if (project.num_of_approvers() == 0) {
                    curr_max_price = project.cost();
                } else {
                    curr_max_price = std::min(
                        static_cast<long long>(project.cost() * pp.num_of_approvers() / project.num_of_approvers()),
                        total_budget); // todo: change if price doesn't have to be long long
                }

                if (pp.num_of_approvers() * project.cost() == project.num_of_approvers() * curr_max_price &&
                    tie_breaking(project, ProjectEmbedding(curr_max_price, pp.name(), pp.approvers()))) {
                    curr_max_price--;
                }

                max_price_to_be_chosen = std::max(max_price_to_be_chosen, curr_max_price);
            }
            total_budget -= project.cost();
        } else if (project == pp) { // not taken because budget too tight
            max_price_to_be_chosen = std::max(max_price_to_be_chosen, total_budget);
        }
    }
    return max_price_to_be_chosen;
}

std::optional<int> optimist_add_for_greedy_over_cost(const Election &election, int p,
                                                     const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto num_of_voters = election.num_of_voters();
    auto projects = election.projects();
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {}; // LCOV_EXCL_LINE (every project should be feasible)

    std::vector<ProjectEmbedding> winners;
    std::ranges::sort(projects, [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        long long cross_term_a_approvals_b_cost = a.num_of_approvers() * b.cost(),
                  cross_term_b_approvals_a_cost = b.num_of_approvers() * a.cost();
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
            if (pp.cost() > total_budget - project.cost()) { // if (last moment to add pp)
                int new_approvers_size = pbmath::ceil_div(project.num_of_approvers() * pp.cost(), project.cost());
                std::vector<int> new_approvers(new_approvers_size);
                std::iota(new_approvers.begin(), new_approvers.end(), 0);
                auto new_pp = ProjectEmbedding(pp.cost(), pp.name(), new_approvers);
                if (project.num_of_approvers() * new_pp.cost() == new_pp.num_of_approvers() * project.cost() &&
                    tie_breaking(project, new_pp)) {
                    new_approvers_size += 1;
                }
                if (new_approvers_size > num_of_voters)
                    return {};
                else
                    return new_approvers_size - pp.num_of_approvers();
            }
            winners.push_back(project);
            total_budget -= project.cost();
        }
    }
    return {}; // LCOV_EXCL_LINE (every project should be feasible)
}

std::optional<int> pessimist_add_for_greedy_over_cost(const Election &election, int p,
                                                      const ProjectComparator &tie_breaking) {
    return optimist_add_for_greedy_over_cost(election, p, tie_breaking);
}

std::optional<int> singleton_add_for_greedy_over_cost(const Election &election, int p,
                                                      const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto projects = election.projects();
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {}; // LCOV_EXCL_LINE (every project should be feasible)

    std::vector<ProjectEmbedding> winners;
    std::ranges::sort(projects, [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        long long cross_term_a_approvals_b_cost = a.num_of_approvers() * b.cost(),
                  cross_term_b_approvals_a_cost = b.num_of_approvers() * a.cost();
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
            if (pp.cost() > total_budget - project.cost()) { // if (last moment to add pp)
                int new_approvers_size = pbmath::ceil_div(project.num_of_approvers() * pp.cost(), project.cost());
                std::vector<int> new_approvers(new_approvers_size);
                std::iota(new_approvers.begin(), new_approvers.end(), 0);
                auto new_pp = ProjectEmbedding(pp.cost(), pp.name(), new_approvers);
                if (project.num_of_approvers() * new_pp.cost() == new_pp.num_of_approvers() * project.cost() &&
                    tie_breaking(project, new_pp)) {
                    new_approvers_size += 1;
                }
                return new_approvers_size - pp.num_of_approvers();
            }
            winners.push_back(project);
            total_budget -= project.cost();
        }
    }
    return {}; // LCOV_EXCL_LINE (every project should be feasible)
}
