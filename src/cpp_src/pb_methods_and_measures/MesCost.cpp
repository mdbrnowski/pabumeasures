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

namespace {
struct Candidate {
    int index;
    long double max_payment_by_cost;

    bool operator>(const Candidate &other) const { return max_payment_by_cost > other.max_payment_by_cost; }
};
} // namespace

std::vector<ProjectEmbedding> mes_cost(const Election &election, const ProjectComparator &tie_breaking) {
    long long total_budget = election.budget();
    long long n_voters = election.numVoters();
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
            long double previous_max_payment_by_cost = current_candidate.max_payment_by_cost;

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

            std::ranges::sort(approvers,
                              [&budget](const long long a, const long long b) { return budget[a] < budget[b]; });

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
