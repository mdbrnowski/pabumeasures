#include "Phragmen.h"

#include "utils/Election.h"
#include "utils/Math.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"

#include <algorithm>
#include <limits>
#include <vector>

std::vector<ProjectEmbedding> phragmen(const Election &election, const ProjectComparator &tie_breaking) {
    // todo: try with max_load recalculation skipping
    int total_budget = election.budget();
    int n_voters = election.numVoters();
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
