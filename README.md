# Pabumeasures

[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/pabumeasures)](https://pypi.org/project/pabumeasures/)
[![PyPI - Version](https://img.shields.io/pypi/v/pabumeasures)](https://pypi.org/project/pabumeasures/)
[![Test](https://github.com/mdbrnowski/pabumeasures/actions/workflows/test.yml/badge.svg)](https://github.com/mdbrnowski/pabumeasures/actions/workflows/test.yml)

## Installation
> Prerequisites
> * [CMake](https://cmake.org/download/)
> * [Google OR-Tools (C++)](https://developers.google.com/optimization/install/cpp) – Install via a package manager (e.g., `brew`) or download a binary distribution.

**Pabumeasures** uses dynamic linking to reduce build times. You might need to make the OR-Tools headers and libraries discoverable at both build-time and runtime by exporting the variables as shown below.

> **Environment Configuration**
>
> ⚠️ **Note:** The path to ortools provided below must be the installation root containing the `lib` and `include` subdirectories.
>
> **Linux**
> ```shell
> export CMAKE_PREFIX_PATH="/path/to/ortools"
> export LD_LIBRARY_PATH="$CMAKE_PREFIX_PATH/lib:$LD_LIBRARY_PATH"
> ```
> **macOS**
> ```shell
> export CMAKE_PREFIX_PATH="/path/to/ortools"
> export DYLD_LIBRARY_PATH="$CMAKE_PREFIX_PATH/lib:$DYLD_LIBRARY_PATH"
> ```
> **Windows**
> ```shell
> set CMAKE_PREFIX_PATH=C:\path\to\ortools
> set PATH=%CMAKE_PREFIX_PATH%\lib;%PATH%
> ```


Then, you can simply install **pabumeasures** from PyPI:

```shell
pip install pabumeasures
```

## Documentation

Currently, there is no dedicated documentation. However, the interface is quite simple.

The general workflow is as follows: create or import PB instances using **pabutools**, then compute rule results and measures for those rules using **pabumeasures**.

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

mes_cost(instance, profile) # returns [p1, p2]
mes_cost_measure(instance, profile, p3, Measure.ADD_APPROVAL_OPTIMIST) # returns 1
```
