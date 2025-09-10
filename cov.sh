#!/usr/bin/env bash

# reset coverage counters
lcov --directory build --zerocounters

# build with coverage flags
CMAKE_ARGS="-DENABLE_COVERAGE=ON" uv pip install -e .

# run tests with Python coverage info
echo "Running pytest"
uv run pytest \
  --maxfail=1 \
  --disable-warnings \
  --quiet \
  --cov=pabumeasures \
  --cov-report=lcov:python_coverage.info \

# capture C++ coverage info
lcov --capture \
     --directory build \
     --base-directory . \
     --output-file coverage.info \
     --filter brace \
     --ignore-errors mismatch,mismatch,gcov,source,source,inconsistent,unsupported

# filter out system and third-party files from C++ coverage info
lcov --remove coverage.info '/usr/*' '*/pybind11/*' '*/uv/python/*' \
     --output-file coverage_filtered.info \
     --ignore-errors unused
rm coverage.info

# fix paths in Python coverage info
sed --in-place "s|SF:|SF:$(pwd)/|g" python_coverage.info

# merge C++ and Python coverage info
lcov -a coverage_filtered.info -a python_coverage.info -o merged.info

# generate html report
genhtml merged.info --output-directory html_coverage --ignore-errors category --prefix `pwd`
