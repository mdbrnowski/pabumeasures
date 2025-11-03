# Pabumeasures

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

## Development

In development, OR-Tools are linked dynamically to make the compilation process faster. It requires precompiled OR-Tools libs. On Linux it's sufficient to download and extract the precompiled libs from [developers.google.com](https://developers.google.com/optimization/install/cpp/binary_linux) and then to run:

```bash
  export ORTOOLS_DIR="PATH/TO/ORTOOLS/DIRECTORY"
  export LD_LIBRARY_PATH=$ORTOOLS_DIR/lib:$LD_LIBRARY_PATH
  export PATH=$ORTOOLS_DIR/bin:$PATH
```

### Release

While preparing the wheels, C++ libraries should be linked statically. To do it, CMake should be called with flag `-DBUILD_STATIC=ON`.

For example if I wanted to run tests on a statically linked version I'd clear the cache and then run

```bash
CMAKE_ARGS="-DBUILD_STATIC=ON" uv run pytest
```
