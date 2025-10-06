# Pabumeasures

## Release
While preparing the wheels, C++ libraries should be linked statically. To do it, CMake should be called with flag `-DBUILD_STATIC=ON`.

For example if I wanted to run tests on a statically compiled version I'd clear the cache and then run 
```bash
CMAKE_ARGS="-DBUILD_STATIC=ON" uv run pytest
```

## Dev
In development, OR-Tools are linked dynamically to make the compilation process faster. It requires precompiled OR-Tools libs. On Linux it's sufficient to download the precompiled libs [from here](https://developers.google.com/optimization/install/cpp/binary_linux) and add their bin folder to the `PATH` system variable. 
