#include "cpp_src/ProjectComparator.h"
#include "cpp_src/ProjectEmbedding.h"
#include <pybind11/native_enum.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <numeric>

using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;

vector<ProjectEmbedding> greedy(int total_budget, vector<ProjectEmbedding> projects,
                                const ProjectComparator &tie_breaking) {
    vector<ProjectEmbedding> winners;
    sort(projects.begin(), projects.end(), [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
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

optional<int> optimist_add_for_greedy(int num_projects, int num_voters, int total_budget, const vector<int> &cost,
                                      const vector<vector<int>> &approvers, int p) {
    if (cost[p] > total_budget)
        return nullopt;
    vector<int> projects(num_projects);
    iota(projects.begin(), projects.end(), 0);
    sort(projects.begin(), projects.end(),
         [&approvers](int a, int b) { return approvers[a].size() > approvers[b].size(); });
    // todo: add tie-breaking
    for (int i = 0; i < num_projects; i++) {
        auto project = projects[i];
        if (cost[project] <= total_budget) {
            if (project == p)
                return 0;
            if (total_budget - cost[project] < cost[p]) {
                int difference = approvers[project].size() - approvers[p].size(); // todo: add tie-breaking
                if (difference + static_cast<int>(approvers[p].size()) > num_voters)
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

vector<ProjectEmbedding> greedy_over_cost(int total_budget, vector<ProjectEmbedding> projects,
                                          const ProjectComparator &tie_breaking) {
    vector<ProjectEmbedding> winners;

    sort(projects.begin(), projects.end(), [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        long long cross_term_a_approvals_b_cost = static_cast<long long>(a.approvers().size()) * b.cost(),
                  cross_term_b_approvals_a_cost = static_cast<long long>(b.approvers().size()) * a.cost();
        if (cross_term_a_approvals_b_cost == cross_term_b_approvals_a_cost) {
            return tie_breaking(a, b);
        }
        return cross_term_a_approvals_b_cost > cross_term_b_approvals_a_cost;
    });
    for (const auto project : projects) {
        if (project.cost() <= total_budget) {
            winners.push_back(project);
            total_budget -= project.cost();
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
    for (int i = 0; i < num_projects; i++) {
        auto project = projects[i];
        if (cost[project] <= total_budget) {
            if (project == p)
                return 0;
            if (total_budget - cost[project] < cost[p]) {
                int difference = ceil(approvers[project].size() / static_cast<double>(cost[project]) *
                                      static_cast<double>(cost[p])) -
                                 approvers[p].size(); // todo: add tie-breaking
                if (difference + static_cast<int>(approvers[p].size()) > num_voters)
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

    py::native_enum<ProjectComparator::Comparator>(m, "Comparator", "enum.Enum")
        .value("COST", ProjectComparator::Comparator::COST)
        .value("VOTES", ProjectComparator::Comparator::VOTES)
        .value("LEXICOGRAPHIC", ProjectComparator::Comparator::LEXICOGRAPHIC)
        .finalize();

    py::native_enum<ProjectComparator::Ordering>(m, "Ordering", "enum.Enum")
        .value("ASCENDING", ProjectComparator::Ordering::ASCENDING)
        .value("DESCENDING", ProjectComparator::Ordering::DESCENDING)
        .finalize();

    py::class_<ProjectEmbedding>(m, "ProjectEmbedding")
        .def(py::init<int, std::string, std::vector<int>>(), "cost"_a, "name"_a, "approvers"_a)
        .def(py::init<int, std::string>(), "cost"_a, "name"_a)
        .def_property_readonly("cost", &ProjectEmbedding::cost)
        .def_property_readonly("name", &ProjectEmbedding::name)
        .def_property_readonly("approvers", &ProjectEmbedding::approvers);

    py::class_<ProjectComparator>(m, "ProjectComparator")
        .def(py::init<std::vector<std::pair<ProjectComparator::Comparator, ProjectComparator::Ordering>>>(),
             "criteria"_a)
        .def(py::init<ProjectComparator::Comparator, ProjectComparator::Ordering>(), "comparator"_a, "ordering"_a)
        .def("__call__", &ProjectComparator::operator())
        // static default comparators
        .def_property_readonly_static("ByCostAsc", [](py::object) { return ProjectComparator::ByCostAsc; })
        .def_property_readonly_static("ByCostDesc", [](py::object) { return ProjectComparator::ByCostDesc; })
        .def_property_readonly_static("ByNameAsc", [](py::object) { return ProjectComparator::ByNameAsc; })
        .def_property_readonly_static("ByNameDesc", [](py::object) { return ProjectComparator::ByNameDesc; })
        .def_property_readonly_static("ByVotesAsc", [](py::object) { return ProjectComparator::ByVotesAsc; })
        .def_property_readonly_static("ByVotesDesc", [](py::object) { return ProjectComparator::ByVotesDesc; })
        .def_property_readonly_static("ByCostAscThenVotesDesc",
                                      [](py::object) { return ProjectComparator::ByCostAscThenVotesDesc; })
        .def_property_readonly_static("ByCostDescThenVotesDesc",
                                      [](py::object) { return ProjectComparator::ByCostDescThenVotesDesc; });

    m.def("greedy", &greedy, "GreedyAV implementation.", "total_budget"_a, "projects"_a, "tie_breaking"_a);

    m.def("optimist_add_for_greedy", &optimist_add_for_greedy, "optimist-add measure for GreedyAV", "num_projects"_a,
          "num_voters"_a, "total_budget"_a, "cost"_a, "approvers"_a, "p"_a);

    m.def("pessimist_add_for_greedy", &pessimist_add_for_greedy, "pessimist-add measure for GreedyAV", "num_projects"_a,
          "num_voters"_a, "total_budget"_a, "cost"_a, "approvers"_a, "p"_a);

    m.def("greedy_over_cost", &greedy_over_cost, "GreedyAV/Cost implementation", "total_budget"_a, "projects"_a,
          "tie_breaking"_a);

    m.def("optimist_add_for_greedy_over_cost", &optimist_add_for_greedy_over_cost,
          "optimist-add measure for GreedyAV/Cost", "num_projects"_a, "num_voters"_a, "total_budget"_a, "cost"_a,
          "approvers"_a, "p"_a);

    m.def("pessimist_add_for_greedy_over_cost", &pessimist_add_for_greedy_over_cost,
          "pessimist-add measure for GreedyAV/Cost", "num_projects"_a, "num_voters"_a, "total_budget"_a, "cost"_a,
          "approvers"_a, "p"_a);
}