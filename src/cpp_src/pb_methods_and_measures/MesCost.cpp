#include "MesCost.h"

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

#include <iostream> // debug

namespace {
struct Candidate {
    int index;
    long double max_payment_by_cost;

    bool operator>(const Candidate &other) const { return max_payment_by_cost > other.max_payment_by_cost; }
};
} // namespace

std::vector<ProjectEmbedding> mes_cost(const Election &election, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.numVoters();
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
        long double min_max_payment_by_cost = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            auto project = projects[current_candidate.index];
            auto previous_max_payment_by_cost = current_candidate.max_payment_by_cost;

            if (pbmath::is_greater_than(previous_max_payment_by_cost, min_max_payment_by_cost)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment_by_cost value can only increase
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
                long double max_payment_by_cost = max_payment / project.cost();
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment_by_cost = max_payment_by_cost;
                    if (pbmath::is_less_than(max_payment_by_cost, min_max_payment_by_cost) ||
                        (pbmath::is_equal(max_payment_by_cost, min_max_payment_by_cost) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment_by_cost !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment_by_cost = max_payment_by_cost;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (min_max_payment_by_cost == std::numeric_limits<long double>::max()) { // No more affordable projects
            break;
        }
        winners.push_back(projects[best_candidate.index]);

        for (const auto &approver : projects[best_candidate.index].approvers()) {
            budget[approver] =
                std::max(0.0L, budget[approver] - min_max_payment_by_cost * projects[best_candidate.index].cost());
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(std::move(candidate));
        }
        candidates_to_reinsert.clear();
    }

    return winners;
}

std::optional<long long> cost_reduction_for_mes_cost(const Election &election, int p,
                                                     const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.numVoters();
    auto projects = election.projects();
    auto pp = projects[p];
    auto pp_approvers = pp.approvers();
    std::optional<long long> max_price_to_be_chosen{};

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> remaining_candidates;

    for (int i = 0; i < projects.size(); i++) {
        remaining_candidates.emplace(i, 0);
    }

    std::vector<long double> budget(n_voters, static_cast<long double>(total_budget) / n_voters);

    std::vector<Candidate> candidates_to_reinsert;
    candidates_to_reinsert.reserve(projects.size());

    while (true) {
        long double min_max_payment_by_cost = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            auto project = projects[current_candidate.index];
            auto previous_max_payment_by_cost = current_candidate.max_payment_by_cost;

            if (pbmath::is_greater_than(previous_max_payment_by_cost, min_max_payment_by_cost)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment_by_cost value can only increase
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
                long double max_payment_by_cost = max_payment / project.cost();
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment_by_cost = max_payment_by_cost;
                    if (pbmath::is_less_than(max_payment_by_cost, min_max_payment_by_cost) ||
                        (pbmath::is_equal(max_payment_by_cost, min_max_payment_by_cost) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment_by_cost !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment_by_cost = max_payment_by_cost;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (min_max_payment_by_cost == std::numeric_limits<long double>::max()) { // No more affordable projects
            long double price_to_be_chosen = 0;
            for (const auto &approver : pp_approvers) {
                price_to_be_chosen += budget[approver];
            }
            price_to_be_chosen =
                pbmath::floor(price_to_be_chosen); // todo: if price doesn't have to be long long, change here

            max_price_to_be_chosen =
                pbmath::optional_max(max_price_to_be_chosen, static_cast<long long>(price_to_be_chosen));

            break;
        }

        auto winner = projects[best_candidate.index];

        if (winner == pp) {
            return pp.cost();
        }

        { // measure calculation
            std::ranges::sort(pp_approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });
            long long price_l = 0, price_r = pp.cost();
            while (price_l + 1 < price_r) {
                long long price_mid = (price_l + price_r) / 2;
                long double curr_min_max_payment = min_max_payment_by_cost * price_mid;
                long double curr_price = 0;
                int voters_left = pp_approvers.size();
                for (const auto &approver : pp_approvers) {
                    if (pbmath::is_less_than(budget[approver], curr_min_max_payment)) {
                        curr_price += budget[approver];
                        voters_left--;
                    } else {
                        curr_price += curr_min_max_payment * voters_left;
                        break;
                    }
                }

                // if(pbmath::is_less_than(curr_price, price_mid)) {
                //     price_l = price_mid;
                //     continue;
                // }

                long double curr_min_max_payment_by_cost = curr_min_max_payment / curr_price;
                if (pbmath::is_greater_than(curr_min_max_payment_by_cost, min_max_payment_by_cost) ||
                    (pbmath::is_equal(curr_min_max_payment_by_cost, min_max_payment_by_cost) &&
                     tie_breaking(winner, ProjectEmbedding(price_mid, pp.name(), pp_approvers)))) {
                    price_r = price_mid;
                } else {
                    price_l = price_mid;
                }
            }
            max_price_to_be_chosen = pbmath::optional_max(max_price_to_be_chosen, price_l);
        }

        for (const auto &approver : winner.approvers()) {
            budget[approver] =
                std::max(0.0L, budget[approver] - min_max_payment_by_cost * projects[best_candidate.index].cost());
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(std::move(candidate));
        }
        candidates_to_reinsert.clear();
    }

    return max_price_to_be_chosen;
}
