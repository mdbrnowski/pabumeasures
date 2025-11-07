#pragma once

#include "utils/Election.h"
#include "utils/ProjectEmbedding.h"

#include <algorithm>
#include <map>
#include <vector>

// Returns pairs of (number of voters of this type, example voter index). Voter type can be identified by the
// intersection of the approval set of a voter and the set of winning projects. We disregard voters that approve p.
// Note: we don't return the type itself since it's not needed in our implementations.
inline std::vector<std::pair<int, int>> calculate_voter_types(const Election &election, int p,
                                                              const std::vector<ProjectEmbedding> &allocation) {
    auto n_voters = election.num_of_voters();
    auto projects = election.projects();

    std::vector<std::vector<int>> approved_projects(n_voters); // only winning ones or those of interest (i.e. p)
    for (int i = 0; i < static_cast<int>(projects.size()); i++) {
        if (p == i || std::ranges::find(allocation, projects[i]) != allocation.end()) {
            for (int approver : projects[i].approvers()) {
                approved_projects[approver].push_back(i);
            }
        }
    }

    std::map<std::vector<int>, std::pair<int, int>> voter_types_map;
    for (int j = 0; j < n_voters; j++) {
        if (std::ranges::find(approved_projects[j], p) == approved_projects[j].end()) {
            voter_types_map[approved_projects[j]].first++;
            voter_types_map[approved_projects[j]].second = j;
        }
    }

    std::vector<std::pair<int, int>> voter_types;
    for (const auto &entry : voter_types_map) {
        voter_types.push_back(entry.second);
    }

    return voter_types;
}
