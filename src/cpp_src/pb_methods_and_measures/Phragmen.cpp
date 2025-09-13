#include "Phragmen.h"

#include "Greedy.h"
#include "utils/Election.h"
#include "utils/Math.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <algorithm>
#include <limits>
#include <numeric>
#include <queue>
#include <unordered_set>
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
            if (project.num_of_approvers() == 0) {
                max_load = std::numeric_limits<long double>::max();
            } else {
                for (const auto &approver : project.approvers())
                    max_load += load[approver];
                max_load /= project.num_of_approvers();
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

        const auto &winner = *std::ranges::min_element(round_winners, tie_breaking);

        for (const auto &approver : winner.approvers()) {
            load[approver] = min_max_load;
        }

        winners.push_back(winner);
        total_budget -= winner.cost();
        projects.erase(remove(projects.begin(), projects.end(), winner), projects.end());
    }
    return winners;
}

long long cost_reduction_for_phragmen(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.numVoters();
    auto projects = election.projects();
    std::vector<long double> load(n_voters, 0);

    auto pp = projects[p];
    long long max_price_to_be_chosen = 0;

    while (!projects.empty()) {
        long double min_max_load = std::numeric_limits<long double>::max();
        std::vector<ProjectEmbedding> round_winners;
        for (const auto &project : projects) {
            long double max_load = project.cost();
            if (project.num_of_approvers() == 0) {
                max_load = std::numeric_limits<long double>::max();
            } else {
                for (const auto &approver : project.approvers())
                    max_load += load[approver];
                max_load /= project.num_of_approvers();
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

        const auto &winner = *std::ranges::min_element(round_winners, tie_breaking);

        if (pp.num_of_approvers() == 0) {
            if (winner.num_of_approvers() == 0 && !would_break_without_pp) {
                int new_p = std::ranges::find(round_winners, pp) - round_winners.begin();
                auto new_election = Election(total_budget, round_winners.size(), round_winners);
                return cost_reduction_for_greedy(new_election, new_p, tie_breaking);
            }
        } else {
            long double load_sum = 0;
            for (const auto &approver : pp.approvers()) {
                load_sum += load[approver];
            }
            long long curr_max_price = pbmath::floor(min_max_load * pp.num_of_approvers() - load_sum);
            curr_max_price = std::min({curr_max_price, pp.cost(), total_budget});
            long double pp_max_load = (curr_max_price + load_sum) / pp.num_of_approvers();

            if (pbmath::is_equal(pp_max_load, min_max_load) &&
                (would_break_without_pp ||
                 tie_breaking(winner, ProjectEmbedding(curr_max_price, pp.name(), pp.approvers())))) {
                curr_max_price--;
            }
            max_price_to_be_chosen = std::max(max_price_to_be_chosen, curr_max_price);
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

std::optional<int> optimist_add_for_phragmen(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.numVoters();
    auto projects = election.projects();
    std::vector<long double> load(n_voters, 0);

    auto pp = projects[p];
    std::unordered_set<int> pp_approvers(pp.approvers().begin(), pp.approvers().end());
    std::optional<int> result{};

    while (!projects.empty()) {
        long double min_max_load = std::numeric_limits<long double>::max();
        std::vector<ProjectEmbedding> round_winners;
        for (const auto &project : projects) {
            long double max_load = project.cost();
            if (project.num_of_approvers() == 0) {
                max_load = std::numeric_limits<long double>::max();
            } else {
                for (const auto &approver : project.approvers())
                    max_load += load[approver];
                max_load /= project.num_of_approvers();
            }

            if (pbmath::is_less_than(max_load, min_max_load)) {
                round_winners.clear();
                min_max_load = max_load;
            }
            if (pbmath::is_equal(max_load, min_max_load)) {
                round_winners.push_back(project);
            }
        }

        if (pp.cost() > total_budget)
            break;

        bool would_break =
            any_of(round_winners.begin(), round_winners.end(),
                   [total_budget](const ProjectEmbedding &winner) { return winner.cost() > total_budget; });

        const auto &winner = *std::ranges::min_element(round_winners, tie_breaking);

        if (winner == pp && !would_break)
            return 0;

        std::priority_queue<std::pair<long double, int>> best_new_approvers;
        for (int i = 0; i < n_voters; i++) {
            if (pp_approvers.find(i) == pp_approvers.end())
                best_new_approvers.push({-load[i], i});
        }
        long double pp_max_load_numerator = pp.cost();
        for (const auto &approver : pp.approvers()) {
            pp_max_load_numerator += load[approver];
        }
        auto new_approvers = pp.approvers();
        bool enough_approvers = true;
        do {
            if (best_new_approvers.empty()) {
                enough_approvers = false;
                break;
            }
            pp_max_load_numerator += load[best_new_approvers.top().second];
            best_new_approvers.pop();
            new_approvers.push_back(best_new_approvers.top().second);
        } while (pbmath::is_greater_than(pp_max_load_numerator / new_approvers.size(), min_max_load) ||
                 (pbmath::is_equal(pp_max_load_numerator / new_approvers.size(), min_max_load) &&
                  (would_break || tie_breaking(winner, ProjectEmbedding(pp.cost(), pp.name(), new_approvers)))));

        if (enough_approvers) {
            result = pbmath::optional_min(result, static_cast<int>(new_approvers.size() - pp.num_of_approvers()));
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
    return result;
}

std::optional<int> singleton_add_for_phragmen(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.numVoters();
    auto projects = election.projects();
    std::vector<long double> load(n_voters, 0);

    auto pp = projects[p];
    std::optional<int> result{};

    while (!projects.empty()) {
        long double min_max_load = std::numeric_limits<long double>::max();
        std::vector<ProjectEmbedding> round_winners;
        for (const auto &project : projects) {
            long double max_load = project.cost();
            if (project.num_of_approvers() == 0) {
                max_load = std::numeric_limits<long double>::max();
            } else {
                for (const auto &approver : project.approvers())
                    max_load += load[approver];
                max_load /= project.num_of_approvers();
            }

            if (pbmath::is_less_than(max_load, min_max_load)) {
                round_winners.clear();
                min_max_load = max_load;
            }
            if (pbmath::is_equal(max_load, min_max_load)) {
                round_winners.push_back(project);
            }
        }

        if (pp.cost() > total_budget)
            break;

        bool would_break =
            any_of(round_winners.begin(), round_winners.end(),
                   [total_budget](const ProjectEmbedding &winner) { return winner.cost() > total_budget; });

        const auto &winner = *std::ranges::min_element(round_winners, tie_breaking);

        if (winner == pp && !would_break)
            return 0;
        long double pp_max_load_numerator = pp.cost();
        for (const auto &approver : pp.approvers()) {
            pp_max_load_numerator += load[approver];
        }
        int new_approvers_size = pbmath::ceil(pp_max_load_numerator / min_max_load);
        std::vector<int> new_approvers(new_approvers_size);
        std::iota(new_approvers.begin(), new_approvers.end(), 0);
        auto pp_max_load = new_approvers_size == 0 ? std::numeric_limits<long double>::max()
                                                   : pp_max_load_numerator / new_approvers_size;
        if (pbmath::is_equal(min_max_load, pp_max_load) &&
            (would_break || tie_breaking(winner, ProjectEmbedding(pp.cost(), pp.name(), new_approvers)))) {
            new_approvers_size += 1;
        }

        result = pbmath::optional_min(result, new_approvers_size - static_cast<int>(pp.num_of_approvers()));

        if (would_break) {
            break;
        }

        for (const auto &approver : winner.approvers()) {
            load[approver] = min_max_load;
        }

        total_budget -= winner.cost();
        projects.erase(remove(projects.begin(), projects.end(), winner), projects.end());
    }
    return result;
}
