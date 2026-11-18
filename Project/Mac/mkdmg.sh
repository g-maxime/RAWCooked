#!/bin/sh

set -e

if [ $# != 3 ]; then
    echo
    echo "Usage: mkdmg.sh appname kind version"
    echo
    exit 1
fi

APPNAME="$1"
KIND="$2"
VERSION="$3"

if [ "$KIND" = "CLI" ] || [ "$KIND" = "cli" ]; then
    KIND="CLI"
elif [ "$KIND" = "GUI" ] || [ "$KIND" = "gui" ]; then
    KIND="GUI"
else
    echo
    echo "KIND must be either [CLI | cli] or [GUI | gui]"
    echo
    exit 1
fi

APPNAME_lower=`echo ${APPNAME} |awk '{print tolower($0)}'`
KIND_lower=`echo ${KIND} |awk '{print tolower($0)}'`
SIGNATURE="MediaArea.net"
GUI_SIGNATURE="${CODESIGN_IDENTITY:-Developer ID Application: ${SIGNATURE}}"
FFMPEG_VERSION="${FFMPEG_VERSION:-9.0.1}"
FILES="tmp-${APPNAME}_${KIND}"
TEMPDMG="tmp-${APPNAME}_${KIND}.dmg"
FINALDMG="${APPNAME/ /}_${KIND}_${VERSION}_Mac.dmg"

# Clean up
rm -fr "${FILES}-Root"
rm -fr "${FILES}"
rm -f "${APPNAME}.pkg"
rm -f "${TEMPDMG}"
rm -f "${FINALDMG}"

echo
echo ========== Create the package ==========
echo

mkdir -p "${FILES}/.background"
cp ../../License.html "${FILES}"
#cp "../../Release/ReadMe_${KIND}_Mac.txt" "${FILES}/ReadMe.txt"
#cp "../../History_${KIND}.txt" "${FILES}/History.txt"
cp Logo_White.icns "${FILES}/.background"

if [ "$KIND" = "CLI" ]; then

    cd ../GNU/CLI
    if test -e ".libs/${APPNAME_lower}"; then
        mv -f ".libs/${APPNAME_lower}" .
    fi
    if ! test -x "${APPNAME_lower}"; then
        echo
        echo "${APPNAME_lower} can’t be found, or this file isn’t a executable."
        echo
        exit 1
    fi
    strip -u -r "${APPNAME_lower}"
    cd ../../Mac

    mkdir -p "${FILES}-Root/usr/local/bin"
    mkdir -p "${FILES}-Root/usr/local/share/man/man1"
    cp "../GNU/CLI/${APPNAME_lower}" "${FILES}-Root/usr/local/bin"
    cp "../../Source/CLI/rawcooked.1" "${FILES}-Root/usr/local/share/man/man1"
    codesign -f --deep --options=runtime  -s "Developer ID Application: ${SIGNATURE}" --verbose "${FILES}-Root/usr/local/bin/${APPNAME_lower}"

    pkgbuild --root "${FILES}-Root" --identifier "net.mediaarea.${APPNAME_lower}.mac-${KIND_lower}" --sign "Developer ID Installer: ${SIGNATURE}" --version "${VERSION}" "${FILES}/${APPNAME_lower}.pkg"
    codesign -f --deep --options=runtime -s "Developer ID Application: ${SIGNATURE}" --verbose "${FILES}/${APPNAME_lower}.pkg"

fi

if [ "$KIND" = "GUI" ]; then

    cd ../Qt
    if ! test -e "${APPNAME}.app"; then
        echo
        echo "${APPNAME}.app can’t be found, or this file isn’t a executable."
        echo
        exit 1
    fi
    cd ../Mac

    cp -r "../Qt/${APPNAME}.app" "${FILES}"

    macdeployqt "${FILES}/${APPNAME}.app"

    if [ "$APPNAME_lower" = "rawcooked" ]; then
        # FFmpeg shared build in the ffmpeg directory next to the program.
        rm -fr "${FILES}/${APPNAME}.app/Contents/MacOS/ffmpeg"
        FFMPEG_ARCHIVE="${FILES}/FFmpeg.zip"
        curl -fsSL \
            -o "${FFMPEG_ARCHIVE}" \
            "https://mediaarea.net/download/binary/ffmpeg/${FFMPEG_VERSION}/FFmpeg_Bin_${FFMPEG_VERSION}_Mac_Shared_universal.zip" || exit 1
        unzip -q "${FFMPEG_ARCHIVE}" -d "${FILES}/${APPNAME}.app/Contents/MacOS/ffmpeg" || exit 1
        rm -f "${FFMPEG_ARCHIVE}"

        # Keep only the library name used by RAWcooked and FFmpeg.
        find "${FILES}/${APPNAME}.app/Contents/MacOS/ffmpeg/lib" -name '*.dylib' ! -name 'lib*.[0-9]*.dylib' -delete
        find "${FILES}/${APPNAME}.app/Contents/MacOS/ffmpeg/lib" -name 'lib*.[0-9]*.[0-9]*.dylib' -delete

        # Use @loader_path so the libraries also work from the application bundle.
        for LIBRARY in "${FILES}/${APPNAME}.app"/Contents/MacOS/ffmpeg/lib/*.dylib ; do
            for DEPENDENCY in $(otool -L "${LIBRARY}" |
                sed -n 's|^[[:space:]]*@executable_path/\.\./lib/\([^ ]*\) .*|\1|p' |
                sort -u) ; do
                install_name_tool \
                    -change "@executable_path/../lib/${DEPENDENCY}" \
                    "@loader_path/../lib/${DEPENDENCY}" \
                    "${LIBRARY}"
            done
        done
    fi

    if [ "${GUI_SIGNATURE}" = "-" ]; then
        codesign -f --deep -s - "${FILES}/${APPNAME}.app"
    else
        codesign -f --deep --options=runtime -s "${GUI_SIGNATURE}" --verbose "${FILES}/${APPNAME}.app"
    fi
    codesign --verify --deep "${FILES}/${APPNAME}.app"
fi

echo
echo ========== Create the disk image ==========
echo

if [ "$KIND" = "GUI" ]; then
    ln -s /Applications "${FILES}/Applications"
    hdiutil create "${FINALDMG}" -ov -fs HFS+ -format UDZO -volname "${APPNAME}" -srcfolder "${FILES}"
    if [ "${GUI_SIGNATURE}" = "-" ]; then
        echo "Not signing the disk image with an ad-hoc identity."
    else
        codesign -f --options=runtime -s "${GUI_SIGNATURE}" --verbose "${FINALDMG}"
    fi
else
    # Check if an old image isn't already attached
    DEVICE=$(hdiutil info |grep -B 1 "/Volumes/${APPNAME}" |egrep '^/dev/' | sed 1q | awk '{print $1}')
    test -e "$DEVICE" && hdiutil detach -force "${DEVICE}"

    hdiutil create "${TEMPDMG}" -ov -fs HFS+ -format UDRW -volname "${APPNAME}" -srcfolder "${FILES}"
    DEVICE=$(hdiutil attach -readwrite -noverify "${TEMPDMG}" | egrep '^/dev/' | sed 1q | awk '{print $1}')
    sleep 2

    cd "/Volumes/${APPNAME}"
    test -e .DS_Store && rm -fr .DS_Store
    cd - >/dev/null

    . Osascript_${KIND}.sh
    osascript_Function

    hdiutil detach "${DEVICE}"
    sleep 2

    echo
    echo ========== Convert to compressed image ==========
    echo
    hdiutil convert "${TEMPDMG}" -format UDBZ -o "${FINALDMG}"
    codesign -f --deep --options=runtime -s "Developer ID Application: ${SIGNATURE}" --verbose "${FINALDMG}"
fi

rm -fr "${FILES}" "${FILES}-Root"

unset -v APPNAME APPNAME_lower KIND KIND_lower VERSION SIGNATURE GUI_SIGNATURE
unset -v FFMPEG_VERSION FFMPEG_ARCHIVE TEMPDMG FINALDMG FILES DEVICE
