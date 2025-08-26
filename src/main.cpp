#include "cpp_src/ProjectComparator.h"
#include "cpp_src/ProjectEmbedding.h"
#include <pybind11/native_enum.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <numeric>

using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;

long long ceil_div(long long a, long long b) { return (a + b - 1) / b; }

int get_number_of_voters(const vector<ProjectEmbedding> &projects) {
    int n_voters = 0;
    for (const auto &project : projects) {
        if (!project.approvers().empty())
            n_voters = max(n_voters, *ranges::max_element(project.approvers()) + 1);
    }
    return n_voters;
}

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

optional<int> optimist_add_for_greedy(int total_budget, vector<ProjectEmbedding> projects, int p,
                                      const ProjectComparator &tie_breaking) {
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {};

    vector<ProjectEmbedding> winners;
    sort(projects.begin(), projects.end(), [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
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
                int num_voters = get_number_of_voters(projects);
                int new_approvers_size = project.approvers().size();
                vector<int> new_approvers(new_approvers_size);
                iota(new_approvers.begin(), new_approvers.end(), 0);
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

optional<int> pessimist_add_for_greedy(int total_budget, vector<ProjectEmbedding> projects, int p,
                                       const ProjectComparator &tie_breaking) {
    return optimist_add_for_greedy(total_budget, projects, p, tie_breaking);
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

optional<int> optimist_add_for_greedy_over_cost(int total_budget, vector<ProjectEmbedding> projects, int p,
                                                const ProjectComparator &tie_breaking) {
    auto pp = projects[p];
    if (pp.cost() > total_budget)
        return {};

    vector<ProjectEmbedding> winners;
    sort(projects.begin(), projects.end(), [&tie_breaking](ProjectEmbedding a, ProjectEmbedding b) {
        long long cross_term_a_approvals_b_cost = static_cast<long long>(a.approvers().size()) * b.cost(),
                  cross_term_b_approvals_a_cost = static_cast<long long>(b.approvers().size()) * a.cost();
        if (cross_term_a_approvals_b_cost == cross_term_b_approvals_a_cost) {
            return tie_breaking(a, b);
        }
        return cross_term_a_approvals_b_cost > cross_term_b_approvals_a_cost;
    });
    for (const auto &project : projects) {
        if (project.cost() <= total_budget) {
            if (project == pp) {
                return 0;
            }
            if (pp.cost() <= total_budget && pp.cost() > total_budget - project.cost()) { // if (last moment to add pp)
                int num_voters = get_number_of_voters(projects);
                int new_approvers_size =
                    ceil_div(static_cast<long long>(project.approvers().size()) * pp.cost(), project.cost());
                vector<int> new_approvers(new_approvers_size);
                iota(new_approvers.begin(), new_approvers.end(), 0);
                auto new_pp = ProjectEmbedding(pp.cost(), pp.name(), new_approvers);
                if (static_cast<long long>(project.approvers().size()) * new_pp.cost() ==
                        static_cast<long long>(new_pp.approvers().size()) * project.cost() &&
                    tie_breaking(project, new_pp)) {
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

optional<int> pessimist_add_for_greedy_over_cost(int total_budget, vector<ProjectEmbedding> projects, int p,
                                                 const ProjectComparator &tie_breaking) {
    return optimist_add_for_greedy_over_cost(total_budget, projects, p, tie_breaking);
}

vector<ProjectEmbedding> phragmen(int total_budget, vector<ProjectEmbedding> projects,
                                  const ProjectComparator &tie_breaking = ProjectComparator::ByCostAsc) {
    vector<ProjectEmbedding> winners;
    int n_voters = get_number_of_voters(projects);
    vector<long double> load(n_voters, 0);
    while (!projects.empty()) {
        long double min_max_load = numeric_limits<long double>::max();
        auto winner = projects[0];
        for (const auto project : projects) {
            long double max_load = project.cost();
            for (const auto &approver : project.approvers())
                max_load += load[approver];
            max_load /= project.approvers().size();

            if ((max_load == min_max_load && tie_breaking(project, winner)) || max_load < min_max_load) {
                min_max_load = max_load;
                winner = project;
            }
        }
        if (winner.cost() > total_budget)
            break;

        for (const auto &approver : winner.approvers()) {
            load[approver] = min_max_load;
        }

        winners.push_back(winner);
        total_budget -= winner.cost();
        projects.erase(remove(projects.begin(), projects.end(), winner), projects.end());
    }
    return winners;
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
        .def_property_readonly_static("ByVotesDesc", [](py::object) { return ProjectComparator::ByVotesDesc; })
        .def_property_readonly_static("ByCostAscThenVotesDesc",
                                      [](py::object) { return ProjectComparator::ByCostAscThenVotesDesc; })
        .def_property_readonly_static("ByCostDescThenVotesDesc",
                                      [](py::object) { return ProjectComparator::ByCostDescThenVotesDesc; });

    m.def("greedy", &greedy, "GreedyAV", "total_budget"_a, "projects"_a, "tie_breaking"_a);

    m.def("optimist_add_for_greedy", &optimist_add_for_greedy, "optimist-add measure for GreedyAV", "total_budget"_a,
          "projects"_a, "p"_a, "tie_breaking"_a);

    m.def("pessimist_add_for_greedy", &pessimist_add_for_greedy, "pessimist-add measure for GreedyAV", "total_budget"_a,
          "projects"_a, "p"_a, "tie_breaking"_a);

    m.def("greedy_over_cost", &greedy_over_cost, "GreedyAV/Cost", "total_budget"_a, "projects"_a, "tie_breaking"_a);

    m.def("optimist_add_for_greedy_over_cost", &optimist_add_for_greedy_over_cost,
          "optimist-add measure for GreedyAV/Cost", "total_budget"_a, "projects"_a, "p"_a, "tie_breaking"_a);

    m.def("pessimist_add_for_greedy_over_cost", &pessimist_add_for_greedy_over_cost,
          "pessimist-add measure for GreedyAV/Cost", "total_budget"_a, "projects"_a, "p"_a, "tie_breaking"_a);

    m.def("phragmen", &phragmen, "Sequential Phragmén", "total_budget"_a, "projects"_a, "tie_breaking"_a);
}
