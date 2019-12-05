#!/usr/bin/env bash

script_path="${PWD}/test"
. ${script_path}/helpers.sh

test="sequence"

pushd "${files_path}" >/dev/null 2>&1
    file=sequence1
    mkdir "${file}"
    ffmpeg -nostdin -f lavfi -i testsrc=duration=5:size=16x16 "${file}/%01d.dpx" >/dev/null 2>&1|| fatal "internal" "ffmpeg command failed"
    run_rawcooked --check "${file}"
    check_success "check failed on valid dpx sequence" "check succeded on valid dpx sequence"

    file=sequence2
    mkdir "${file}"
    ffmpeg -nostdin -f lavfi -i testsrc=duration=0.200:size=16x16 -start_number 9 "${file}/%01d.dpx" >/dev/null 2>&1|| fatal "internal" "ffmpeg command failed"
    run_rawcooked --check "${file}"
    check_failure "check failed due to incomplete dpx sequence" "check succeded despite incomplete dpx sequence"

    clean
popd >/dev/null 2>&1

exit ${status}
