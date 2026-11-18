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

macosx_version_min="11.0"
artifact="${release_directory}/RAWcooked_GUI_${version}_Mac.dmg"
application="${project_directory}/Project/Qt/RAWcooked.app"

#-----------------------------------------------------------------------
# Cleanup
rm -f "${artifact}"

#-----------------------------------------------------------------------
# Build RAWcooked GUI
pushd "${project_directory}/Project/Qt" > /dev/null
    sh prepare \
        rawcooked-gui.pro \
        "QMAKE_APPLE_DEVICE_ARCHS=x86_64 arm64" \
        "QMAKE_MACOSX_DEPLOYMENT_TARGET=${macosx_version_min}"
    make -j"$(sysctl -n hw.ncpu)"
popd > /dev/null

if [ ! -d "${application}" ]; then
    echo "The RAWcooked application bundle was not found."
    exit 1
fi

architectures="$(lipo -archs "${application}/Contents/MacOS/RAWcooked")"
echo "RAWcooked GUI architectures: ${architectures}"
case " ${architectures} " in
    *" x86_64 "*) ;;
    *)
        echo "The RAWcooked GUI binary has no x86_64 architecture."
        exit 1
        ;;
esac
case " ${architectures} " in
    *" arm64 "*) ;;
    *)
        echo "The RAWcooked GUI binary has no arm64 architecture."
        exit 1
        ;;
esac

#-----------------------------------------------------------------------
# Package the application and disk image
pushd "${project_directory}/Project/Mac" > /dev/null
    ./mkdmg.sh RAWcooked GUI "${version}"
popd > /dev/null

#-----------------------------------------------------------------------
# Copy artifact
cp -a "${project_directory}/Project/Mac/RAWcooked_GUI_${version}_Mac.dmg" "${release_directory}/"
