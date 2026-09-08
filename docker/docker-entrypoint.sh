#!/bin/sh
[ -z $CONFIG ] && config=Release || config="$CONFIG"

# sampgdk (submodule) codegen needs python + ply; python3 is installed in the
# image, ply is installed user-local (ENV PATH includes ~/.local/bin).
python3 -m pip install --user ply || exit 1

cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=$config \
    -DSAMPGDK_ARCH=${ARCH:-64} \
    -DPYTHON_EXECUTABLE=$(command -v python3) \
&&
cmake \
    --build build \
    --config $config \
    --parallel $(nproc) \
&& cd build && cpack -B package