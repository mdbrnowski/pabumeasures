#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace pybind11::literals;
namespace py = pybind11;

std::vector<int> greedy() {
  return std::vector<int>({1, 2, 3, 4, 5});
}

PYBIND11_MODULE(_core, m) {
  m.doc() = "core module with all internal functions";

  m.def("greedy", &greedy, R"pbdoc(
      GreedyAV implementation.
  )pbdoc");
}
