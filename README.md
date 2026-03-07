# Pabumeasures

[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/pabumeasures)](https://pypi.org/project/pabumeasures/)
[![PyPI - Version](https://img.shields.io/pypi/v/pabumeasures)](https://pypi.org/project/pabumeasures/)
[![Test](https://github.com/mdbrnowski/pabumeasures/actions/workflows/test.yml/badge.svg)](https://github.com/mdbrnowski/pabumeasures/actions/workflows/test.yml)

**Pabumeasures** implements the project performance measures from [*Evaluation of Project Performance in Participatory Budgeting*](https://arxiv.org/abs/2312.14723) (Boehmer et al., arXiv:2312.14723) for the GreedyAV, Greedy over Cost, Phragmén, and Method of Equal Shares rules with both cost and approval-based utility functions.
The package is fully compatible with [pabutools](https://github.com/COMSOC-Community/pabutools), using its `Instance`, `ApprovalProfile`, and `BudgetAllocation` types.

## Installation

**Prerequisites**

* [CMake](https://cmake.org/download/),
* [Google OR-Tools (C++)](https://developers.google.com/optimization/install/cpp) – install via a package manager (e.g., `brew`) or download a binary distribution.

Pabumeasures uses dynamic linking to reduce build times. You might need to make the OR-Tools headers and libraries discoverable at both build-time and runtime by exporting the variables as shown below.

> [!NOTE]
> The path to OR-Tools provided below must be the installation root containing the `lib` and `include` subdirectories.
>
> **Linux**
>
> ```shell
> export CMAKE_PREFIX_PATH="/path/to/ortools"
> export LD_LIBRARY_PATH="$CMAKE_PREFIX_PATH/lib:$LD_LIBRARY_PATH"
> ```
>
> **macOS**
>
> ```shell
> export CMAKE_PREFIX_PATH="/path/to/ortools"
> export DYLD_LIBRARY_PATH="$CMAKE_PREFIX_PATH/lib:$DYLD_LIBRARY_PATH"
> ```
>
> **Windows**
>
> ```shell
> set CMAKE_PREFIX_PATH=C:\path\to\ortools
> set PATH=%CMAKE_PREFIX_PATH%\lib;%PATH%
> ```

Then, you can simply install pabumeasures from PyPI:

```shell
pip install pabumeasures
```

## Documentation

The general workflow is: create or import PB instances using [pabutools](https://github.com/COMSOC-Community/pabutools), then compute rule outcomes and measures using pabumeasures.

### Example

```py
from pabumeasures import Measure, mes_cost, mes_cost_measure
from pabutools.election import ApprovalBallot, ApprovalProfile, Instance, Project

p1 = Project("p1", 1)
p2 = Project("p2", 1)
p3 = Project("p3", 3)

b1 = ApprovalBallot([p1, p2])
b2 = ApprovalBallot([p1, p2, p3])
b3 = ApprovalBallot([p3])

instance = Instance([p1, p2, p3], budget_limit=3)
profile = ApprovalProfile([b1, b2, b3])

mes_cost(instance, profile)  # returns BudgetAllocation([p1, p2])
mes_cost_measure(instance, profile, p3, Measure.ADD_APPROVAL_OPTIMIST)  # returns 1
```

### Available functions

#### Rules

| pabumeasures | pabutools equivalent |
| --- | --- |
| `greedy(instance, profile)` | `greedy_utilitarian_welfare(instance, profile, sat_class=Cost_Sat)` |
| `greedy_over_cost(instance, profile)` | `greedy_utilitarian_welfare(instance, profile, sat_class=Cardinality_Sat)` |
| `mes_cost(instance, profile)` | `method_of_equal_shares(instance, profile, sat_class=Cost_Sat)` |
| `mes_apr(instance, profile)` | `method_of_equal_shares(instance, profile, sat_class=Cardinality_Sat)` |
| `phragmen(instance, profile)` | `sequential_phragmen(instance, profile)` |

All rule functions return a `BudgetAllocation` and accept an optional `tie_breaking: ProjectComparator` (default: `ProjectComparator.ByCostAsc`).

#### Measures

Measure functions have no pabutools equivalent.

| Function | Description |
| --- | --- |
| `greedy_measure(instance, profile, project, measure)` | Measure for GreedyAV |
| `greedy_over_cost_measure(instance, profile, project, measure)` | Measure for Greedy over Cost |
| `mes_cost_measure(instance, profile, project, measure)` | Measure for MES-Cost |
| `mes_apr_measure(instance, profile, project, measure)` | Measure for MES-Apr |
| `phragmen_measure(instance, profile, project, measure)` | Measure for sequential Phragmén |

All measure functions return an `int` (or `None` if the measure is not defined) and accept an optional `tie_breaking: ProjectComparator`. The `measure` argument is one of (see [Boehmer et al.](https://arxiv.org/abs/2312.14723) for formal definitions):

| `Measure` value | Name in paper |
| --- | --- |
| `Measure.COST_REDUCTION` | cost-red |
| `Measure.ADD_APPROVAL_OPTIMIST` | optimist-add |
| `Measure.ADD_APPROVAL_PESSIMIST` | pessimist-add |
| `Measure.ADD_SINGLETON` | singleton-add |
