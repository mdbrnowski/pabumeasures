#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <numeric>

using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;

vector<int> greedy(int num_projects, int num_voters, int total_budget, const vector<int> &cost, const vector<vector<int>> &approvers) {
    vector<int> winners, projects(num_projects);
    iota(projects.begin(), projects.end(), 0);
    sort(projects.begin(), projects.end(),
         [&approvers](int a, int b) { return approvers[a].size() > approvers[b].size(); });
    // todo: add tie-breaking
    for (const auto project : projects) {
        if (cost[project] < total_budget) {
            winners.push_back(project);
            total_budget -= cost[project];
        }
        if (total_budget <= 0)
            break;
    }
    return winners;
}

PYBIND11_MODULE(_core, m) {
    m.doc() = "core module with all internal functions";

    m.def("greedy", &greedy, "GreedyAV implementation.", "num_projects"_a, "num_voters"_a, "total_budget"_a, "cost"_a,
          "approvers"_a);
}
