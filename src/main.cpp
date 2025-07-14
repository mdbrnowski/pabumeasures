#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <numeric>

using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;

vector<int> greedy(int num_projects, int num_voters, int total_budget, const vector<int> &cost,
                   const vector<vector<int>> &approvers) {
    vector<int> winners, projects(num_projects);
    iota(projects.begin(), projects.end(), 0);
    sort(projects.begin(), projects.end(),
         [&approvers](int a, int b) { return approvers[a].size() > approvers[b].size(); });
    // todo: add tie-breaking
    for (const auto project : projects) {
        if (cost[project] <= total_budget) {
            winners.push_back(project);
            total_budget -= cost[project];
        }
        if (total_budget <= 0)
            break;
    }
    return winners;
}

optional<int> optimist_add_for_greedy(int num_projects, int num_voters, int total_budget, const vector<int> &cost,
                                      const vector<vector<int>> &approvers, int p) {
    if (cost[p] > total_budget)
        return nullopt;
    vector<int> projects(num_projects);
    iota(projects.begin(), projects.end(), 0);
    sort(projects.begin(), projects.end(),
         [&approvers](int a, int b) { return approvers[a].size() > approvers[b].size(); });
    // todo: add tie-breaking
    for (size_t i = 0; i < projects.size(); i++) {
        auto project = projects[i];
        if (cost[project] <= total_budget) {
            if (project == p)
                return 0;
            if (total_budget - cost[project] < cost[p]) {
                int difference = approvers[project].size() - approvers[p].size(); // todo: add tie-breaking
                if (difference + approvers[p].size() > num_voters)
                    return nullopt;
                else
                    return difference;
            }
            total_budget -= cost[project];
        }
        if (total_budget <= 0)
            break;
    }
    return nullopt;
}

optional<int> pessimist_add_for_greedy(int num_projects, int num_voters, int total_budget, const vector<int> &cost,
                                       const vector<vector<int>> &approvers, int p) {
    return optimist_add_for_greedy(num_projects, num_voters, total_budget, cost, approvers, p);
}

vector<int> greedy_over_cost(int num_projects, int num_voters, int total_budget, const vector<int> &cost,
                             const vector<vector<int>> &approvers) {
    vector<int> winners, projects(num_projects);
    iota(projects.begin(), projects.end(), 0);
    sort(projects.begin(), projects.end(), [&approvers, &cost](int a, int b) {
        return approvers[a].size() / static_cast<double>(cost[a]) > approvers[b].size() / static_cast<double>(cost[b]);
    });
    // todo: add tie-breaking
    for (const auto project : projects) {
        if (cost[project] <= total_budget) {
            winners.push_back(project);
            total_budget -= cost[project];
        }
        if (total_budget <= 0)
            break;
    }
    return winners;
}

optional<int> optimist_add_for_greedy_over_cost(int num_projects, int num_voters, int total_budget,
                                                const vector<int> &cost, const vector<vector<int>> &approvers, int p) {
    if (cost[p] > total_budget)
        return nullopt;
    vector<int> projects(num_projects);
    iota(projects.begin(), projects.end(), 0);
    sort(projects.begin(), projects.end(), [&approvers, &cost](int a, int b) {
        return approvers[a].size() / static_cast<double>(cost[a]) > approvers[b].size() / static_cast<double>(cost[b]);
    });
    // todo: add tie-breaking
    for (size_t i = 0; i < projects.size(); i++) {
        auto project = projects[i];
        if (cost[project] <= total_budget) {
            if (project == p)
                return 0;
            if (total_budget - cost[project] < cost[p]) {
                int difference = ceil(approvers[project].size() / static_cast<double>(cost[project]) *
                                      static_cast<double>(cost[p])) -
                                 approvers[p].size(); // todo: add tie-breaking
                if (difference + approvers[p].size() > num_voters)
                    return nullopt;
                else
                    return difference;
            }
            total_budget -= cost[project];
        }
        if (total_budget <= 0)
            break;
    }
    return nullopt;
}

optional<int> pessimist_add_for_greedy_over_cost(int num_projects, int num_voters, int total_budget,
                                                 const vector<int> &cost, const vector<vector<int>> &approvers, int p) {
    return optimist_add_for_greedy(num_projects, num_voters, total_budget, cost, approvers, p);
}

PYBIND11_MODULE(_core, m) {
    m.doc() = "core module with all internal functions";

    m.def("greedy", &greedy, "GreedyAV implementation.", "num_projects"_a, "num_voters"_a, "total_budget"_a, "cost"_a,
          "approvers"_a);

    m.def("optimist_add_for_greedy", &optimist_add_for_greedy, "optimist-add measure for GreedyAV", "num_projects"_a,
          "num_voters"_a, "total_budget"_a, "cost"_a, "approvers"_a, "p"_a);

    m.def("pessimist_add_for_greedy", &pessimist_add_for_greedy, "pessimist-add measure for GreedyAV", "num_projects"_a,
          "num_voters"_a, "total_budget"_a, "cost"_a, "approvers"_a, "p"_a);

    m.def("greedy_over_cost", &greedy_over_cost, "GreedyAV/Cost implementation", "num_projects"_a, "num_voters"_a,
          "total_budget"_a, "cost"_a, "approvers"_a);

    m.def("optimist_add_for_greedy_over_cost", &optimist_add_for_greedy_over_cost,
          "optimist-add measure for GreedyAV/Cost", "num_projects"_a, "num_voters"_a, "total_budget"_a, "cost"_a,
          "approvers"_a, "p"_a);

    m.def("pessimist_add_for_greedy_over_cost", &pessimist_add_for_greedy_over_cost,
          "pessimist-add measure for GreedyAV/Cost", "num_projects"_a, "num_voters"_a, "total_budget"_a, "cost"_a,
          "approvers"_a, "p"_a);
}
