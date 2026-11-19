#!/usr/bin/env bash
# Build the pinned toolchain on the arm64 macOS CI runner.
set -eu -o pipefail
[[ $(uname -sm) == "Darwin arm64" ]]
work=${1:?provide a toolchain build directory}
mkdir -p "$work"
work=$(cd "$work" && pwd)
brew install llvm@21 lld libomp ninja
llvm=$(brew --prefix llvm@21)
lld=$(brew --prefix lld)/bin
if [[ ! -x "$lld/ld64.lld" ]]; then
    mkdir -p "$work/linker"
    "$lld/lld" -flavor darwin --version
    ln -sf "$lld/lld" "$work/linker/ld64.lld"
    lld="$work/linker"
fi
checkout() {
    local url=$1 revision=$2 path=$3
    if [[ ! -d "$path/.git" ]]; then
        git init -q "$path"
        git -C "$path" fetch --depth=1 "$url" "$revision"
        git -C "$path" checkout -q --detach FETCH_HEAD
    fi
    [[ $(git -C "$path" rev-parse HEAD) == "$revision" ]]
    git -C "$path" submodule update --init --recursive --depth 1
}
checkout https://github.com/AdaptiveCpp/AdaptiveCpp.git 96c7d17a9d029a5660ac06d7519b3763e9823ab6 "$work/acpp"
checkout https://github.com/apple/metal-cpp.git 8cdde68ae36a636c84cbb48ebcbc8f3b60251ea0 "$work/metal-cpp"
cmake -S "$work/acpp" -B "$work/build" -G Ninja -DCMAKE_BUILD_TYPE=Release \
    "-DCMAKE_INSTALL_PREFIX=$work/install" "-DCMAKE_OSX_SYSROOT=$(xcrun --show-sdk-path)" \
    "-DCMAKE_C_COMPILER=$llvm/bin/clang" "-DCMAKE_CXX_COMPILER=$llvm/bin/clang++" \
    "-DLLVM_DIR=$llvm/lib/cmake/llvm" "-DClang_DIR=$llvm/lib/cmake/clang" \
    "-DACPP_LLD_PATH=$lld/ld64.lld" "-DCMAKE_PREFIX_PATH=$(brew --prefix libomp)" \
    "-DCMAKE_SHARED_LINKER_FLAGS=-Wl,-needed_framework,CoreGraphics" \
    "-DCMAKE_MODULE_LINKER_FLAGS=-Wl,-needed_framework,CoreGraphics" \
    -DWITH_SSCP_COMPILER=ON -DWITH_ACCELERATED_CPU=OFF -DWITH_STDPAR_COMPILER=OFF \
    -DWITH_METAL_BACKEND=ON "-DMETAL_INCLUDE_DIR=$work/metal-cpp" \
    -DDEFAULT_TARGETS=generic -DWITH_OPENCL_BACKEND=OFF -DWITH_CUDA_BACKEND=OFF -DWITH_ROCM_BACKEND=OFF
cmake --build "$work/build" --parallel 3
CMAKE_INSTALL_ALWAYS=1 cmake --install "$work/build"
