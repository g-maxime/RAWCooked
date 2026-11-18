##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
##
##  Use of this source code is governed by a BSD-style license that can
##  be found in the License.html file in the root of the source tree.
##

$ErrorActionPreference = "Stop"

#-----------------------------------------------------------------------
# Setup
$release_directory = $PSScriptRoot
$version = (Get-Content (Join-Path (Split-Path $release_directory -Parent) "Project\version.txt") -Raw).Trim()

$arch = "x64"
$arch_alt = $arch

#-----------------------------------------------------------------------
# Build
 Push-Location (Join-Path (Split-Path $release_directory -Parent) "Project\Qt")
    qmake
    nmake
Pop-Location

#-----------------------------------------------------------------------
# Export uninstaller
Push-Location $release_directory
    makensis.exe -DEXPORT_UNINST "..\Source\Install\RAWcooked_GUI_Windows_${arch_alt}.nsi"
Pop-Location
