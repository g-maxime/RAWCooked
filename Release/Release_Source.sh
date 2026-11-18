#!/bin/bash

##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 #  Use of this source code is governed by a BSD-style license that can
 #  be found in the License.html file in the root of the source tree.
 ##

set -e # fail on any error

#-----------------------------------------------------------------------
# Setup
release_directory="$(realpath -- "$(dirname -- "${BASH_SOURCE[0]}")")"
version="$(<"${release_directory}/../Project/version.txt")"

temp_directory="$(mktemp -d)"
trap 'rm -rf -- "${temp_directory}"' EXIT

#-----------------------------------------------------------------------
# Source archives
pushd "${temp_directory}"
    mkdir rawcooked
    cp -r "${release_directory}/.." rawcooked

    # Remove what is not wanted
    rm -rf rawcooked/.git*

    # Make source archives
    GZIP=-9 tar -cz --owner=root --group=root -f "rawcooked_${version}.tar.gz" rawcooked
    BZIP=-9 tar -cj --owner=root --group=root -f "rawcooked_${version}.tar.bz2" rawcooked
    XZ_OPT=-9e tar -cJ --owner=root --group=root -f "rawcooked_${version}.tar.xz" rawcooked
    7za a -t7z -mx=9 -bd "rawcooked_${version}.7z" rawcooked
popd


#-----------------------------------------------------------------------
# Unix CLI archives
pushd "${temp_directory}"
    mkdir RAWcooked_CLI_GNU_FromSource
    cp -r "${release_directory}/.." RAWcooked_CLI_GNU_FromSource

    # Script preparation
    mv RAWcooked_CLI_GNU_FromSource/Project/GNU/CLI/AddThisToRoot_CLI_compile.sh RAWcooked_CLI_GNU_FromSource/CLI_Compile.sh
    chmod +x RAWcooked_CLI_GNU_FromSource/CLI_Compile.sh
    chmod +x RAWcooked_CLI_GNU_FromSource/Project/GNU/CLI/autogen.sh
    chmod +x RAWcooked_CLI_GNU_FromSource/Project/Mac/mkdmg.sh

    # Autotools
    pushd RAWcooked_CLI_GNU_FromSource/Project/GNU/CLI
        ./autogen.sh
    popd

    # Remove what is not wanted
    rm -rf RAWcooked_CLI_GNU_FromSource/.git*
    rm -rf RAWcooked_CLI_GNU_FromSource/debian
    rm -rf RAWcooked_CLI_GNU_FromSource/Project/Chocolatey
    rm -rf RAWcooked_CLI_GNU_FromSource/Project/MSVC2022
    rm -rf RAWcooked_CLI_GNU_FromSource/Project/MSVC2026
    rm -rf RAWcooked_CLI_GNU_FromSource/Project/Qt
    rm -rf RAWcooked_CLI_GNU_FromSource/Project/Mac/*_GUI.sh
    rm -rf RAWcooked_CLI_GNU_FromSource/Source/GUI
    rm -rf RAWcooked_CLI_GNU_FromSource/Source/Resource
    rm -rf RAWcooked_CLI_GNU_FromSource/Source/Install

    # Make source archives
    GZIP=-9 tar -cz --owner=root --group=root -f "RAWcooked_CLI_${version}_GNU_FromSource.tar.gz" RAWcooked_CLI_GNU_FromSource
    BZIP=-9 tar -cj --owner=root --group=root -f "RAWcooked_CLI_${version}_GNU_FromSource.tar.bz2" RAWcooked_CLI_GNU_FromSource
    XZ_OPT=-9e tar -cJ --owner=root --group=root -f "RAWcooked_CLI_${version}_GNU_FromSource.tar.xz" RAWcooked_CLI_GNU_FromSource
popd

#-----------------------------------------------------------------------
# Unix GUI archives
pushd "${temp_directory}"
    mkdir RAWcooked_GUI_GNU_FromSource
    cp -r "${release_directory}/.." RAWcooked_GUI_GNU_FromSource

    # Script preparation
    mv RAWcooked_GUI_GNU_FromSource/Project/Qt/AddThisToRoot_GUI_compile.sh RAWcooked_GUI_GNU_FromSource/GUI_Compile.sh
    chmod +x RAWcooked_GUI_GNU_FromSource/GUI_Compile.sh
    chmod +x RAWcooked_GUI_GNU_FromSource/Project/Mac/mkdmg.sh

    # Remove what is not wanted
    rm -rf RAWcooked_GUI_GNU_FromSource/.git*
    rm -f RAWcooked_GUI_GNU_FromSource/History_CLI.txt
    rm -rf RAWcooked_CLI_GNU_FromSource/debian
    rm -rf RAWcooked_CLI_GNU_FromSource/Project/Chocolatey
    rm -rf RAWcooked_CLI_GNU_FromSource/Project/MSVC2022
    rm -rf RAWcooked_CLI_GNU_FromSource/Project/MSVC2026
    rm -rf RAWcooked_GUI_GNU_FromSource/Source/CLI
    rm -rf RAWcooked_GUI_GNU_FromSource/Source/Install
    rm -rf RAWcooked_GUI_GNU_FromSource/Project/Mac/*_CLI.sh


    # Make source archives
    GZIP=-9 tar -cz --owner=root --group=root -f "RAWcooked_GUI_${version}_GNU_FromSource.tar.gz" RAWcooked_GUI_GNU_FromSource
    BZIP=-9 tar -cj --owner=root --group=root -f "RAWcooked_GUI_${version}_GNU_FromSource.tar.bz2" RAWcooked_GUI_GNU_FromSource
    XZ_OPT=-9e tar -cJ --owner=root --group=root -f "RAWcooked_GUI_${version}_GNU_FromSource.tar.xz" RAWcooked_GUI_GNU_FromSource
popd

#-----------------------------------------------------------------------
# Copy the source packages to the source directory
mkdir "${release_directory}/source"
cp "${temp_directory}/rawcooked_${version}.tar.gz" "${release_directory}/source"
cp "${temp_directory}/rawcooked_${version}.tar.bz2" "${release_directory}/source"
cp "${temp_directory}/rawcooked_${version}.tar.xz" "${release_directory}/source"
cp "${temp_directory}/rawcooked_${version}.7z" "${release_directory}/source"
cp "${temp_directory}/RAWcooked_CLI_${version}_GNU_FromSource.tar.gz" "${release_directory}/source"
cp "${temp_directory}/RAWcooked_CLI_${version}_GNU_FromSource.tar.bz2" "${release_directory}/source"
cp "${temp_directory}/RAWcooked_CLI_${version}_GNU_FromSource.tar.xz" "${release_directory}/source"
cp "${temp_directory}/RAWcooked_GUI_${version}_GNU_FromSource.tar.gz" "${release_directory}/source"
cp "${temp_directory}/RAWcooked_GUI_${version}_GNU_FromSource.tar.bz2" "${release_directory}/source"
cp "${temp_directory}/RAWcooked_GUI_${version}_GNU_FromSource.tar.xz" "${release_directory}/source"
