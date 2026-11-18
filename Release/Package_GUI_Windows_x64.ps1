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
# Cleanup
$artifact = Join-Path $release_directory "RAWcooked_GUI_${version}_Windows_${arch_alt}"
if (Test-Path $artifact) {
    Remove-Item $artifact -Force -Recurse
}

$artifact = Join-Path $release_directory "RAWcooked_GUI_${version}_Windows_${arch_alt}.zip"
if (Test-Path $artifact) {
    Remove-Item $artifact -Force
}

$artifact = Join-Path $release_directory "RAWcooked_GUI_${version}_Windows_${arch_alt}.exe"
if (Test-Path $artifact) {
    Remove-Item $artifact -Force
}

#-----------------------------------------------------------------------
# Package gui
Push-Location "${release_directory}"
    New-Item -ItemType Directory -Path "RAWcooked_GUI_${version}_Windows_${arch_alt}" -Force

    Push-Location "RAWcooked_GUI_${version}_Windows_${arch_alt}"
        Copy-Item (Join-Path (Split-Path $release_directory -Parent) "Project\Qt\${arch}\RAWcooked-GUI.exe") -Force
        Copy-Item (Join-Path (Split-Path $release_directory -Parent) "License.html") -Force

        & 7za.exe a -r -tzip "..\RAWcooked_GUI_${version}_Windows_${arch_alt}.zip" *
    Pop-Location
Pop-Location

#-----------------------------------------------------------------------
# Package installer
Push-Location "${release_directory}"
    makensis.exe "..\Source\Install\RAWcooked_GUI_Windows_${arch_alt}.nsi"
Pop-Location
