# Pabumeasures

[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/pabumeasures)](https://pypi.org/project/pabumeasures/)
[![PyPI - Version](https://img.shields.io/pypi/v/pabumeasures)](https://pypi.org/project/pabumeasures/)
[![Test](https://github.com/mdbrnowski/pabumeasures/actions/workflows/test.yml/badge.svg)](https://github.com/mdbrnowski/pabumeasures/actions/workflows/test.yml)

## Installation

To speed up compilation, OR-Tools (a dependency of **pabumeasures**) is linked dynamically and must be available on your system, so you should install the C++ version of OR-Tools from the official Google [OR-Tools website](https://developers.google.com/optimization/install/cpp).
After that, ensure that the headers and shared libraries can be found during installation and at runtime, e.g. by setting the following environment variables:

```shell
export ORTOOLS_DIR="PATH/TO/ORTOOLS/DIRECTORY"
export LD_LIBRARY_PATH=$ORTOOLS_DIR/lib:$LD_LIBRARY_PATH
export PATH=$ORTOOLS_DIR/bin:$PATH
```

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
