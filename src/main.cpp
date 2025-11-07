#include "cpp_src/pb_rules_and_measures/Greedy.h"
#include "cpp_src/pb_rules_and_measures/GreedyOverCost.h"
#include "cpp_src/pb_rules_and_measures/MesApr.h"
#include "cpp_src/pb_rules_and_measures/MesCost.h"
#include "cpp_src/pb_rules_and_measures/Phragmen.h"
#include "cpp_src/utils/Election.h"
#include "cpp_src/utils/ProjectComparator.h"
#include "cpp_src/utils/ProjectEmbedding.h"
#include <pybind11/native_enum.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;

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
        .def(py::init<long long, std::string, std::vector<int>>(), "cost"_a, "name"_a, "approvers"_a)
        .def_property_readonly("cost", &ProjectEmbedding::cost)
        .def_property_readonly("name", &ProjectEmbedding::name)
        .def_property_readonly("approvers", &ProjectEmbedding::approvers)
        .def_property_readonly("num_of_approvers", &ProjectEmbedding::num_of_approvers);

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

    py::class_<Election>(m, "Election")
        .def(py::init<long long, int, std::vector<ProjectEmbedding>>(), "budget"_a, "num_of_voters"_a, "projects"_a)
        .def_property_readonly("budget", &Election::budget)
        .def_property_readonly("num_of_voters", &Election::num_of_voters)
        .def_property_readonly("projects", &Election::projects);

    m.def("greedy", &greedy, "GreedyAV", "election"_a, "tie_breaking"_a);

    m.def("cost_reduction_for_greedy", &cost_reduction_for_greedy, "Cost reduction measure for GreedyAV", "election"_a,
          "p"_a, "tie_breaking"_a);

    m.def("optimist_add_for_greedy", &optimist_add_for_greedy, "optimist-add measure for GreedyAV", "election"_a, "p"_a,
          "tie_breaking"_a);

    m.def("pessimist_add_for_greedy", &pessimist_add_for_greedy, "pessimist-add measure for GreedyAV", "election"_a,
          "p"_a, "tie_breaking"_a);

    m.def("singleton_add_for_greedy", &singleton_add_for_greedy, "singleton-add measure for GreedyAV", "election"_a,
          "p"_a, "tie_breaking"_a);

    m.def("greedy_over_cost", &greedy_over_cost, "GreedyAV/Cost", "election"_a, "tie_breaking"_a);

    m.def("cost_reduction_for_greedy_over_cost", &cost_reduction_for_greedy_over_cost,
          "Cost reduction measure for GreedyAV/Cost", "election"_a, "p"_a, "tie_breaking"_a);

    m.def("optimist_add_for_greedy_over_cost", &optimist_add_for_greedy_over_cost,
          "optimist-add measure for GreedyAV/Cost", "election"_a, "p"_a, "tie_breaking"_a);

    m.def("pessimist_add_for_greedy_over_cost", &pessimist_add_for_greedy_over_cost,
          "pessimist-add measure for GreedyAV/Cost", "election"_a, "p"_a, "tie_breaking"_a);

    m.def("singleton_add_for_greedy_over_cost", &singleton_add_for_greedy_over_cost,
          "singleton-add measure for GreedyAV/Cost", "election"_a, "p"_a, "tie_breaking"_a);

    m.def("mes_apr", &mes_apr, "Method of Equal Shares with approval utilities", "election"_a, "tie_breaking"_a);

    m.def("cost_reduction_for_mes_apr", &cost_reduction_for_mes_apr,
          "Cost reduction measure for Method of Equal Shares with approval utilities", "election"_a, "p"_a,
          "tie_breaking"_a);

    m.def("optimist_add_for_mes_apr", &optimist_add_for_mes_apr,
          "Optimist-add measure for Method of Equal Shares with approval utilities", "election"_a, "p"_a,
          "tie_breaking"_a);

    m.def("pessimist_add_for_mes_apr", &pessimist_add_for_mes_apr,
          "Pessimist-add measure for Method of Equal Shares with approval utilities", "election"_a, "p"_a,
          "tie_breaking"_a);

    m.def("singleton_add_for_mes_apr", &singleton_add_for_mes_apr,
          "Singleton-add measure for Method of Equal Shares with approval utilities", "election"_a, "p"_a,
          "tie_breaking"_a);

    m.def("mes_cost", &mes_cost, "Method of Equal Shares with cost utilities", "election"_a, "tie_breaking"_a);

    m.def("cost_reduction_for_mes_cost", &cost_reduction_for_mes_cost,
          "Cost reduction measure for Method of Equal Shares with cost utilities", "election"_a, "p"_a,
          "tie_breaking"_a);

    m.def("optimist_add_for_mes_cost", &optimist_add_for_mes_cost,
          "Optimist-add measure for Method of Equal Shares with cost utilities", "election"_a, "p"_a, "tie_breaking"_a);

    m.def("pessimist_add_for_mes_cost", &pessimist_add_for_mes_cost,
          "Pessimist-add measure for Method of Equal Shares with cost utilities", "election"_a, "p"_a,
          "tie_breaking"_a);

    m.def("singleton_add_for_mes_cost", &singleton_add_for_mes_cost,
          "Singleton-add measure for Method of Equal Shares with cost utilities", "election"_a, "p"_a,
          "tie_breaking"_a);

    m.def("phragmen", &phragmen, "Sequential Phragmén", "election"_a, "tie_breaking"_a);

    m.def("cost_reduction_for_phragmen", &cost_reduction_for_phragmen, "Cost reduction measure for Sequential Phragmén",
          "election"_a, "p"_a, "tie_breaking"_a);

    m.def("optimist_add_for_phragmen", &optimist_add_for_phragmen, "Optimist-add measure for Sequential Phragmén",
          "election"_a, "p"_a, "tie_breaking"_a);

    m.def("pessimist_add_for_phragmen", &pessimist_add_for_phragmen, "Pessimist-add measure for Sequential Phragmén",
          "election"_a, "p"_a, "tie_breaking"_a);

    m.def("singleton_add_for_phragmen", &singleton_add_for_phragmen, "Singleton-add measure for Sequential Phragmén",
          "election"_a, "p"_a, "tie_breaking"_a);
}
