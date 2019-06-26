#!/usr/bin/env bash

script_path="${PWD}/test"
. ${script_path}/helpers.sh

test="ecc"

# local helper functions
modify_value() {
    printf "%02x" "$((0x${1}^255))"
}

swap_byte() {
    local pos="${1}"
    local file="${2}"
    local buffer="$(xxd -u -p -l 1 -s ${pos} ${file})" || return 1
    buffer="$(printf %02x $((0x${buffer}^255)))" || return 1
    echo ${buffer} | xxd -r -p -l 1 -s ${pos} - ${file} || return 1
}

pushd "${files_path}" >/dev/null 2>&1
    mkdir small
    run_ffmpeg ffmpeg -f lavfi -i color=c=black -vframes 1 small/small_%04d.dpx || fatal "internal" "ffmpeg command failed"
    run_rawcooked --ecc small/ || fatal "internal" "rawcooked command failed"

    if [ "$(find small -maxdepth 1 | wc -l)" -gt "2" ] ; then
        echo "NOK: ${test}/small, garbages in source directory" >&${fd}
        status=1
    fi
    rm -fr small
    mv -f small.mkv small.mkv.orig

    mkdir big
    run_ffmpeg ffmpeg -nostdin -f lavfi -i anoisesrc=duration=3500 big/big.wav || fatal "internal" "ffmpeg command failed"
    run_rawcooked -c:a copy big/
    run_rawcooked --check --ecc big.mkv

    if [ "$(find big -maxdepth 1 | wc -l)" -gt "2" ] ; then
        echo "NOK: ${test}/big, garbages in source directory" >&${fd}
        status=1
    fi

    if [ "$(${fsize} big.mkv)" -lt 314572800 ] ; then
        echo "NOK: ${test}/big, file too small" >&${fd}
        status=1
    fi
    rm -fr big
    mv -f big.mkv big.mkv.orig

    for file in small big ; do
        size="$(${fsize} ${file}.mkv.orig)"
        min=60
        max=$((size-(1048576*8)))

        if [ "${size}" -lt "$((min+(1048576*8)))" ] ; then
            echo "NOK: ${test}/${file}, file too small" >&${fd}
            status=1
            continue
        fi

        pos=$min
        cp -f ${file}.mkv.orig ${file}.mkv
        for count in $(seq 0 9) ; do
            if [ "${pos}" -gt "${max}" ] ; then
                break
            fi

            if [ "${count}" -gt 0 ] ; then
                swap_byte ${pos} ${file}.mkv
                pos=$((pos+(1048576*10)))
            fi

            cp -f ${file}.mkv ${file}-${count}.mkv
            run_rawcooked --check ${file}-${count}.mkv
            if [ "${count}" -lt 9 ] ; then
                if ! contains "Warning: non-conforming input file, it is corrupted" "${cmd_stderr}" ; then
                    if [ "${count}" -gt 0 ] ; then
                        echo "NOK: ${test}/${file}, corruption not detected with --check option" >&${fd}
                        status=1
                    fi
                fi

                chmod 0440 ${file}-${count}.mkv
                run_rawcooked --fix ${file}-${count}.mkv
                if ! contains "Error: undecodable file" "${cmd_stderr}" ; then
                    echo "NOK: ${test}/${file}, read-only file accepted with --fix option" >&${fd}
                    status=1
                fi

                chmod 0660 ${file}-${count}.mkv
                run_rawcooked --fix ${file}-${count}.mkv
                check_files ${file}-${count}.mkv ${file}.mkv.orig
            else
                if ! contains "Error: undecodable input file, it is corrupted" "${cmd_stderr}" ; then
                    echo "NOK: ${test}/${file}, uncorrectable corruption not detected with --check option" >&${fd}
                    status=1
                fi
            fi
            rm -f ${file}-${count}.mkv
        done
        rm -f ${file}.mkv

        if [ ${max} -gt "$((1048577+(1048576*8)))" ] ; then
            cp -f ${file}.mkv.orig ${file}.mkv
            dd if=/dev/zero of=${file}.mkv seek=1048576 bs=1 count=$((1048576*8)) conv=notrunc >/dev/null 2>&1 || fatal "internal" "dd command failed"
            run_rawcooked --check ${file}.mkv
            if ! contains "Warning: non-conforming input file, it is corrupted" "${cmd_stderr}" ; then
                echo "NOK: ${test}/${file}, 8MB corruption at 0x100000 not detected with --check option" >&${fd}
                status=1
            fi

            cp -f ${file}.mkv.orig ${file}.mkv
            dd if=/dev/zero of=${file}.mkv seek=1048577 bs=1 count=$((1048576*7)) conv=notrunc >/dev/null 2>&1 || fatal "internal" "dd command failed"
            run_rawcooked --check ${file}.mkv
            if ! contains "Warning: non-conforming input file, it is corrupted" "${cmd_stderr}" ; then
                echo "NOK: ${test}/${file}, 7MB corruption at 0x100001 not detected with --check option" >&${fd}
                status=1
            fi

            cp -f ${file}.mkv.orig ${file}.mkv
            dd if=/dev/zero of=${file}.mkv seek=1048577 bs=1 count=$((1048576*8)) conv=notrunc >/dev/null 2>&1 || fatal "internal" "dd command failed"
            run_rawcooked --check ${file}.mkv
            if ! contains "Error: undecodable input file, it is corrupted" "${cmd_stderr}" ; then
                echo "NOK: ${test}/${file}, 8MB corruption at 0x100001 not detected with --check option" >&${fd}
                status=1
            fi
        fi
    done
    clean
popd >/dev/null 2>&1

exit ${status}
