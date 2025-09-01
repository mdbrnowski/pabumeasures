#include "MesApr.h"

#include "utils/Election.h"
#include "utils/Math.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <ranges>
#include <vector>

struct Candidate {
    int index;
    long double max_payment;

    bool operator>(const Candidate &other) const { return max_payment > other.max_payment; }
};

std::vector<ProjectEmbedding> mes_apr(const Election &election, const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    int n_voters = election.numVoters();
    auto projects = election.projects();

    std::vector<ProjectEmbedding> winners;

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> remaining_candidates;

    for (int i = 0; i < projects.size(); i++) {
        remaining_candidates.emplace(i, 0);
    }

    std::vector<long double> budget(n_voters, static_cast<long double>(total_budget) / n_voters);

    std::vector<Candidate> candidates_to_reinsert;
    candidates_to_reinsert.reserve(projects.size());

    while (true) {
        long double min_max_payment = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            auto project = projects[current_candidate.index];
            long double previous_max_payment = current_candidate.max_payment;

            if (pbmath::is_greater_than(previous_max_payment, min_max_payment)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment value can only increase
            }

            long double money_behind_project = 0;
            auto approvers = project.approvers();

            for (const auto &approver : approvers) {
                money_behind_project += budget[approver];
            }

            if (pbmath::is_less_than(money_behind_project, project.cost())) {
                continue;
            }

            std::ranges::sort(approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

            long double paid_so_far = 0, denominator = approvers.size();

            for (const auto &approver : approvers) {
                long double max_payment = (static_cast<long double>(project.cost()) - paid_so_far) / denominator;
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment = max_payment;
                    if (pbmath::is_less_than(max_payment, min_max_payment) ||
                        (pbmath::is_equal(max_payment, min_max_payment) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment = max_payment;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (min_max_payment == std::numeric_limits<long double>::max()) { // No more affordable projects
            break;
        }
        winners.push_back(projects[best_candidate.index]);

        for (const auto &approver : projects[best_candidate.index].approvers()) {
            budget[approver] = std::max(0.0L, budget[approver] - min_max_payment);
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(std::move(candidate));
        }
        candidates_to_reinsert.clear();
    }

    return winners;
}

std::optional<int> cost_reduction_for_mes_apr(const Election &election, int p, const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    int n_voters = election.numVoters();
    auto projects = election.projects();
    auto pp = projects[p];
    auto pp_approvers = pp.approvers();
    std::optional<int> max_price_to_be_chosen{};

    if (pp.cost() > total_budget)
        return {}; // LCOV_EXCL_LINE (every project should be feasible)

    if (pp_approvers.size() == 0)
        return {};

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> remaining_candidates;

    for (int i = 0; i < projects.size(); i++) {
        remaining_candidates.emplace(i, 0);
    }

    std::vector<long double> budget(n_voters, static_cast<long double>(total_budget) / n_voters);

    std::vector<Candidate> candidates_to_reinsert;
    candidates_to_reinsert.reserve(projects.size());

    while (true) {
        long double min_max_payment = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            auto project = projects[current_candidate.index];
            long double previous_max_payment = current_candidate.max_payment;

            if (pbmath::is_greater_than(previous_max_payment, min_max_payment)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment value can only increase
            }

            long double money_behind_project = 0;
            auto approvers = project.approvers();

            for (const auto &approver : approvers) {
                money_behind_project += budget[approver];
            }

            if (pbmath::is_less_than(money_behind_project, project.cost())) {
                continue;
            }

            std::ranges::sort(approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

            long double paid_so_far = 0, denominator = approvers.size();

            for (const auto &approver : approvers) {
                long double max_payment = (static_cast<long double>(project.cost()) - paid_so_far) / denominator;
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment = max_payment;
                    if (pbmath::is_less_than(max_payment, min_max_payment) ||
                        (pbmath::is_equal(max_payment, min_max_payment) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment = max_payment;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (min_max_payment == std::numeric_limits<long double>::max()) { // No more affordable projects
            long double price_to_be_chosen = 0;
            for (const auto &approver : pp_approvers) {
                price_to_be_chosen += budget[approver];
            }
            price_to_be_chosen =
                pbmath::floor(price_to_be_chosen); // todo: if price doesn't have to be int, change here
            if (max_price_to_be_chosen) {
                *max_price_to_be_chosen = std::max(*max_price_to_be_chosen, static_cast<int>(price_to_be_chosen));
            } else {
                max_price_to_be_chosen = static_cast<int>(price_to_be_chosen);
            }
            break;
        }

        if (best_candidate.index == p) {
            return pp.cost();
        }

        auto winner = projects[best_candidate.index];

        { // measure calculation
            std::ranges::sort(pp_approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

            long double price_to_be_chosen = 0, full_participators_number = pp_approvers.size();
            for (const auto &approver : pp_approvers) {
                if (pbmath::is_greater_than(min_max_payment, budget[approver])) { // cannot afford to fully participate
                    price_to_be_chosen += budget[approver];
                    full_participators_number--;
                } else {
                    price_to_be_chosen += full_participators_number * min_max_payment;
                    break;
                }
            }

            long double floored_price_to_be_chosen =
                pbmath::floor(price_to_be_chosen); // todo: if price doesn't have to be int, change here
            if (pbmath::is_equal(floored_price_to_be_chosen, price_to_be_chosen) &&
                tie_breaking(winner, ProjectEmbedding(floored_price_to_be_chosen, pp.name(), pp_approvers))) {
                floored_price_to_be_chosen--;
            }

            if (max_price_to_be_chosen) {
                *max_price_to_be_chosen =
                    std::max(*max_price_to_be_chosen, static_cast<int>(floored_price_to_be_chosen));
            } else {
                max_price_to_be_chosen = static_cast<int>(floored_price_to_be_chosen);
            }
        }

        for (const auto &approver : winner.approvers()) {
            budget[approver] = std::max(0.0L, budget[approver] - min_max_payment);
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(std::move(candidate));
        }
        candidates_to_reinsert.clear();
    }

    return max_price_to_be_chosen;
}