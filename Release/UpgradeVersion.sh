#!/bin/bash

##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 #  Use of this source code is governed by a BSD-style license that can
 #  be found in the License.html file in the root of the source tree.
 ##

set -e # fail on any error

if [ "$#" -ne 1 ]; then
    echo "Usage: ${0} version"
    exit 1
fi

if ! [[ "$1" =~ ^[0-9]+\.[0-9]+(\.[0-9]+)?(\.[0-9]+)?$ ]]; then
    echo "Error: version must be in the format Major.Minor[.Build[.Patch]]"
    exit 1
fi

#-----------------------------------------------------------------------
# Setup
release_directory="$(realpath -- "$(dirname -- "${BASH_SOURCE[0]}")")"
version_new="${1}"
version_new_major="$(echo "${version_new}" | cut -d. -f1)"
version_new_minor="$(echo "${version_new}" | cut -d. -f2)"
version_new_build="$(echo "${version_new}" | cut -d. -f3)"
version_new_patch="$(echo "${version_new}" | cut -d. -f4)"
version_old="$(<"${release_directory}/../Project/version.txt")"
version_old_major="$(echo "${version_old}" | cut -d. -f1)"
version_old_minor="$(echo "${version_old}" | cut -d. -f2)"
version_old_build="$(echo "${version_old}" | cut -d. -f3)"
version_old_patch="$(echo "${version_old}" | cut -d. -f4)"

#-----------------------------------------------------------------------
# Files containing the public Major.Minor version
files_dot=(
    "${release_directory}/../Project/version.txt"
    "${release_directory}/../Project/Chocolatey/rawcooked.nuspec"
    "${release_directory}/../Project/Chocolatey/tools/chocolateyinstall.ps1"
    "${release_directory}/../Project/OBS/rawcooked.spec"
    "${release_directory}/../Project/OBS/rawcooked.dsc"
    "${release_directory}/../Project/OBS/PKGBUILD"
    "${release_directory}/../Project/Mac/Info.plist"
    "${release_directory}/../Project/Qt/rawcooked-gui.metainfo.xml"
    "${release_directory}/../Source/CLI/rawcooked.1"
    "${release_directory}/../Source/GUI/Qt/GUI_Package.h"
    "${release_directory}/../Source/Install/RAWcooked_GUI_Windows_x64.nsi"
    "${release_directory}/../Source/Lib/Compressed/RAWcooked/RAWcooked.cpp"
)

for file in "${files_dot[@]}" ; do
   sed -i "s/${version_old//./\\.}/${version_new}/g" "${file}"
done

#-----------------------------------------------------------------------
# Windows resource files use four numeric components
files_rc=(
    "${release_directory}/../Project/MSVC2022/CLI/RAWcooked.rc"
    "${release_directory}/../Project/MSVC2026/CLI/RAWcooked.rc"
    "${release_directory}/../Project/Qt/rawcooked-gui.rc"
)

for file in "${files_rc[@]}" ; do
    sed -i "s/${version_old_major}\\.${version_old_minor}\\.${version_old_build:-0}\\.${version_old_patch:-0}/${version_new_major}.${version_new_minor}.${version_new_build:-0}.${version_new_patch:-0}/g" "${file}"
done

for file in "${files_rc[@]}" ; do
    sed -i "s/${version_old_major},${version_old_minor},${version_old_build:-0},${version_old_patch:-0}/${version_new_major},${version_new_minor},${version_new_build:-0},${version_new_patch:-0}/g" "${file}"
done

#-----------------------------------------------------------------------
# Processing nsi VERSION4
if [ -n "${version_new_patch}" ] ; then
    sed -i "s/!define PRODUCT_VERSION4 \"\${PRODUCT_VERSION}[0-9.]*\"/!define PRODUCT_VERSION4 \"\${PRODUCT_VERSION}\"/g" \
    "${release_directory}/../Source/Install/RAWcooked_GUI_Windows_x64.nsi"
elif [ -n "${version_new_build}" ] ; then
    sed -i "s/!define PRODUCT_VERSION4 \"\${PRODUCT_VERSION}[0-9.]*\"/!define PRODUCT_VERSION4 \"\${PRODUCT_VERSION}.0\"/g" \
    "${release_directory}/../Source/Install/RAWcooked_GUI_Windows_x64.nsi"
else
    sed -i "s/!define PRODUCT_VERSION4 \"\${PRODUCT_VERSION}[0-9.]*\"/!define PRODUCT_VERSION4 \"\${PRODUCT_VERSION}.0.0\"/g" \
    "${release_directory}/../Source/Install/RAWcooked_GUI_Windows_x64.nsi"
fi

#-----------------------------------------------------------------------
# Update changelogs
date_rfc2822="$(LC_ALL=C date -u -R)"
debian_changelog="${release_directory}/../debian/changelog"
cat - <<EOF > "${debian_changelog}.new"
rawcooked (${version_new}-1) stable; urgency=medium

  * Update to version ${version_new}
  * See history for more info

 -- MediaArea CI <info@mediaarea.net>  ${date_rfc2822}

EOF

sed -i "1e cat ${debian_changelog}.new" "${debian_changelog}"
rm -f "${debian_changelog}.new"

#-----------------------------------------------------------------------
# Update OBS changelog
date_rpm="$(LC_ALL=C date -u '+%a %b %e %Y' | sed 's/  / /g')"
rpm_changelog="${release_directory}/../Project/OBS/rawcooked.changes"
cat - <<EOF > "${rpm_changelog}.new"
* ${date_rpm} MediaArea CI <info@mediaarea.net> - ${version_new}
- Version ${version_new}
- See History_CLI.txt for more information

EOF

sed -i "1e cat ${rpm_changelog}.new" "${rpm_changelog}"
rm -f "${rpm_changelog}.new"

#-----------------------------------------------------------------------
# Update metainfo release date
date_YYYY_MM_DD="$(LC_ALL=C date -u +%Y-%m-%d)"
sed -i "s/date=\"[0-9-]\+\"/date=\"${date_YYYY_MM_DD}\"/g" \
    "${release_directory}/../Project/Qt/rawcooked-gui.metainfo.xml"
