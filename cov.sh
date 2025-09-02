#!/usr/bin/env bash
lcov --directory build --zerocounters
CMAKE_ARGS="-DENABLE_COVERAGE=ON" uv pip install -e .
echo "Running pytest"
uv run pytest --maxfail=1 --disable-warnings --quiet
lcov --capture --directory build \
     --output-file coverage.info \
     --filter brace \
     --ignore-errors mismatch,mismatch,gcov,source,source,inconsistent,unsupported
lcov --remove coverage.info '/usr/*' '*/pybind11/*' '*/uv/python/*' \
     -o coverage_filtered.info --ignore-errors unused

genhtml coverage_filtered.info --output-directory cpp_coverage_html --ignore-errors category

