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

#-----------------------------------------------------------------------
# Create obs repository
mkdir "${release_directory}/obs"

cp "${release_directory}/../Project/OBS/rawcooked.spec" "${release_directory}/obs/"
cp "${release_directory}/../Project/OBS/rawcooked.changes" "${release_directory}/obs/"
cp "${release_directory}/../Project/OBS/rawcooked.dsc" "${release_directory}/obs/"
cp "${release_directory}/../Project/OBS/PKGBUILD" "${release_directory}/obs/"

cp "${release_directory}/../debian/rawcooked.dirs" "${release_directory}/obs/debian.rawcooked.dirs"
cp "${release_directory}/../debian/rawcooked.install" "${release_directory}/obs/debian.rawcooked.install"
cp "${release_directory}/../debian/rawcooked.manpages" "${release_directory}/obs/debian.rawcooked.manpages"
cp "${release_directory}/../debian/rawcooked-gui.dirs" "${release_directory}/obs/debian.rawcooked-gui.dirs"
cp "${release_directory}/../debian/rawcooked-gui.install" "${release_directory}/obs/debian.rawcooked-gui.install"
cp "${release_directory}/../debian/changelog" "${release_directory}/obs/debian.changelog"
cp "${release_directory}/../debian/compat" "${release_directory}/obs/debian.compat"
cp "${release_directory}/../debian/control" "${release_directory}/obs/debian.control"
cp "${release_directory}/../debian/copyright" "${release_directory}/obs/debian.copyright"
cp "${release_directory}/../debian/rules" "${release_directory}/obs/debian.rules"

cp "${release_directory}/source/rawcooked_${version}.tar.xz" "${release_directory}/obs/"
