#!/bin/bash

##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 #  Use of this source code is governed by a BSD-style license that can
 #  be found in the License.html file in the root of the source tree.
 ##

set -euo pipefail

#-----------------------------------------------------------------------
# Setup
release_directory="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
project_directory="$(dirname -- "${release_directory}")"
version="$(tr -d '\r\n' < "${project_directory}/Project/version.txt")"

macosx_version_min="10.9"
artifact="${release_directory}/RAWcooked_CLI_${version}_Mac.dmg"

#-----------------------------------------------------------------------
# Cleanup
rm -f "${artifact}"

#-----------------------------------------------------------------------
# Build RAWcooked CLI
pushd "${project_directory}/Project/GNU/CLI" > /dev/null
    sh autogen.sh
    ./configure \
        --enable-universal_binary \
        --with-macosx-version-min="${macosx_version_min}"
    make -j"$(sysctl -n hw.ncpu)"
popd > /dev/null

rawcooked_binary="${project_directory}/Project/GNU/CLI/rawcooked"
if [ ! -x "${rawcooked_binary}" ] && [ -x "${project_directory}/Project/GNU/CLI/.libs/rawcooked" ]; then
    rawcooked_binary="${project_directory}/Project/GNU/CLI/.libs/rawcooked"
fi
if [ ! -x "${rawcooked_binary}" ]; then
    echo "The RAWcooked CLI binary was not found."
    exit 1
fi

architectures="$(lipo -archs "${rawcooked_binary}")"
echo "RAWcooked CLI architectures: ${architectures}"
case " ${architectures} " in
    *" x86_64 "*) ;;
    *)
        echo "The RAWcooked CLI binary has no x86_64 architecture."
        exit 1
        ;;
esac
case " ${architectures} " in
    *" arm64 "*) ;;
    *)
        echo "The RAWcooked CLI binary has no arm64 architecture."
        exit 1
        ;;
esac

#-----------------------------------------------------------------------
# Package .pkg and .dmg
pushd "${project_directory}/Project/Mac" > /dev/null
    ./mkdmg.sh RAWcooked CLI "${version}"
popd > /dev/null

#-----------------------------------------------------------------------
# Copy artifact
cp -a "${project_directory}/Project/Mac/RAWcooked_CLI_${version}_Mac.dmg" "${release_directory}/"
