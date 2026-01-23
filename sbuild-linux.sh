#!/bin/sh

set -x

BUILD_TYPE=Release
if [ "$#" -eq 1 ]; then
    BUILD_TYPE=$1
fi

export JOBS=4
export IBIS_OCIO=ON
export IBIS_JPEG=ON
export IBIS_TIFF=ON
export IBIS_EXR=ON
export IBIS_FFMPEG=ON
export IBIS_FFMPEG_MINIMAL=OFF
export IBIS_OIIO=ON
export IBIS_USD=OFF
export IBIS_PYTHON=OFF
export IBIS_TESTS=ON
export IBIS_GCOV=OFF
export FTK_API=GL_4_1
export BUILD_SHARED_LIBS=OFF

sh ibis/etc/Linux/linux-build-gha.sh $BUILD_TYPE
