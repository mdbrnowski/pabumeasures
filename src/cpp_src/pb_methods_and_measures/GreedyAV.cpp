#include "GreedyAV.h"
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
    std::sort(projects.begin(), projects.end(), [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
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

std::optional<int> optimist_add_for_greedy(const Election &election, int p, const ProjectComparator &tie_breaking) {
    int total_budget = election.budget();
    int num_voters = election.numVoters();
    auto projects = election.projects();
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {};

    std::vector<ProjectEmbedding> winners;
    std::sort(projects.begin(), projects.end(), [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
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
            if (pp.cost() <= total_budget && pp.cost() > total_budget - project.cost()) { // if (last moment to add pp)
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
        if (total_budget <= 0)
            break;
    }
    return {}; // only if project p is not feasible (won't happen)
}

std::optional<int> pessimist_add_for_greedy(const Election &election, int p, const ProjectComparator &tie_breaking) {
    return optimist_add_for_greedy(election, p, tie_breaking);
}