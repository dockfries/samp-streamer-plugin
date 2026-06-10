#!/bin/sh
[ -z $CONFIG ] && config=Release || config="$CONFIG"

cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=$config \
    -DSAMPGDK_ARCH=${ARCH:-64} \
&&
cmake \
    --build build \
    --config $config \
    --parallel $(nproc) \
&& cd build && cpack -B package