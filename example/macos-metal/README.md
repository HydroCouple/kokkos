# Apple silicon Metal build

A Metal-enabled AdaptiveCpp compiler and its runtime are required. Put `acpp` on PATH and select a macOS SDK supported by its LLVM installation using `SDKROOT`. From the fork root:

```sh
cmake --preset macos-arm64-metal -DCMAKE_INSTALL_PREFIX="$PWD/build/metal-install"
cmake --build --preset macos-arm64-metal
cmake --install build/macos-arm64-metal
cmake -S example/macos-metal -B build/metal-smoke -G Ninja \
  -DCMAKE_CXX_COMPILER=acpp -DCMAKE_CXX_FLAGS=--acpp-targets=generic \
  -DCMAKE_PREFIX_PATH="$PWD/build/metal-install"
cmake --build build/metal-smoke
build/metal-smoke/kokkos-metal-smoke metal
```

The macOS workflow installs and tests the package and archives it as a build artifact. Its hosted test verifies an installed-package consumer on the explicit SYCL CPU control. For GPU verification, manually dispatch the hardware job on a registered physical runner labeled `metal` with CMake, Ninja and Metal-enabled `acpp` on PATH. The GPU test requires a Metal queue and checks every output of a 4,099-element range loop.

The OpenSWMM integration uses FP32 elementwise kernels and copies. Cross-workgroup sums, minima and prefix scans have produced incorrect results on the M1 Max and remain experimental. The artifact requires a compatible AdaptiveCpp toolchain/runtime.
