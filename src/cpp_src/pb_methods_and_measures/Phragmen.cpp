#include "Phragmen.h"

#include "Greedy.h"
#include "utils/Election.h"
#include "utils/Math.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <algorithm>
#include <limits>
#include <vector>

std::vector<ProjectEmbedding> phragmen(const Election &election, const ProjectComparator &tie_breaking) {
    // todo: try with max_load recalculation skipping
    auto total_budget = election.budget();
    auto n_voters = election.numVoters();
    auto projects = election.projects();
    std::vector<ProjectEmbedding> winners;
    std::vector<long double> load(n_voters, 0);
    while (!projects.empty()) {
        long double min_max_load = std::numeric_limits<long double>::max();
        std::vector<ProjectEmbedding> round_winners;
        for (const auto &project : projects) {
            long double max_load = project.cost();
            if (project.approvers().empty()) {
                max_load = std::numeric_limits<long double>::max();
            } else {
                for (const auto &approver : project.approvers())
                    max_load += load[approver];
                max_load /= project.approvers().size();
            }

            if (pbmath::is_less_than(max_load, min_max_load)) {
                round_winners.clear();
                min_max_load = max_load;
            }
            if (pbmath::is_equal(max_load, min_max_load)) {
                round_winners.push_back(project);
            }
        }
        if (any_of(round_winners.begin(), round_winners.end(),
                   [total_budget](const ProjectEmbedding &winner) { return winner.cost() > total_budget; })) {
            break;
        }

        auto winner = *std::ranges::min_element(round_winners, tie_breaking);

        for (const auto &approver : winner.approvers()) {
            load[approver] = min_max_load;
        }

        winners.push_back(winner);
        total_budget -= winner.cost();
        projects.erase(remove(projects.begin(), projects.end(), winner), projects.end());
    }
    return winners;
}

std::optional<long long> cost_reduction_for_phragmen(const Election &election, int p,
                                                     const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.numVoters();
    auto projects = election.projects();
    auto pp = projects[p];

    std::vector<long double> load(n_voters, 0);
    std::optional<long long> max_price_to_be_chosen = 0;

    while (!projects.empty()) {
        long double min_max_load = std::numeric_limits<long double>::max();
        std::vector<ProjectEmbedding> round_winners;
        for (const auto &project : projects) {
            long double max_load = project.cost();
            if (project.approvers().empty()) {
                max_load = std::numeric_limits<long double>::max();
            } else {
                for (const auto &approver : project.approvers())
                    max_load += load[approver];
                max_load /= project.approvers().size();
            }

            if (pbmath::is_less_than(max_load, min_max_load)) {
                round_winners.clear();
                min_max_load = max_load;
            }
            if (pbmath::is_equal(max_load, min_max_load)) {
                round_winners.push_back(project);
            }
        }

        bool would_break =
            any_of(round_winners.begin(), round_winners.end(),
                   [total_budget](const ProjectEmbedding &winner) { return winner.cost() > total_budget; });
        bool would_break_without_pp =
            any_of(round_winners.begin(), round_winners.end(), [total_budget, &pp](const ProjectEmbedding &winner) {
                return winner.cost() > total_budget && !(winner == pp);
            });

        auto winner = *std::ranges::min_element(round_winners, tie_breaking);

        if (pp.approvers().empty()) {
            if (winner.approvers().empty() && !would_break_without_pp) {
                int new_p = std::ranges::find(round_winners, pp) - round_winners.begin();
                auto new_election = Election(total_budget, round_winners.size(), round_winners);
                return cost_reduction_for_greedy(new_election, new_p, tie_breaking);
            }
        } else {
            long double load_sum = 0;
            for (const auto &approver : pp.approvers()) {
                load_sum += load[approver];
            }
            long long curr_max_price = pbmath::floor(min_max_load * pp.approvers().size() - load_sum);
            curr_max_price = std::min({curr_max_price, pp.cost(), total_budget});
            long double pp_max_load = (curr_max_price + load_sum) / pp.approvers().size();

            if (pbmath::is_equal(pp_max_load, min_max_load) &&
                (would_break_without_pp ||
                 tie_breaking(winner, ProjectEmbedding(curr_max_price, pp.name(), pp.approvers())))) {
                curr_max_price--;
            }
            max_price_to_be_chosen = pbmath::optional_max(max_price_to_be_chosen, curr_max_price);
        }

        if (would_break) {
            break;
        }

        for (const auto &approver : winner.approvers()) {
            load[approver] = min_max_load;
        }

        total_budget -= winner.cost();
        projects.erase(remove(projects.begin(), projects.end(), winner), projects.end());
    }
    return max_price_to_be_chosen;
}

std::optional<int> singleton_add_for_phragmen(const Election &election, int p, const ProjectComparator &tie_breaking) {
    return {};
};
