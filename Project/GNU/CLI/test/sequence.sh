#!/usr/bin/env bash

script_path="${PWD}/test"
. ${script_path}/helpers.sh

test="sequence"

pushd "${files_path}" >/dev/null 2>&1
    mkdir sequence1
    ffmpeg -f lavfi -i testsrc=duration=5:size=16x16 sequence1/%01d.dpx >&$fd
    ls sequence1 >&$fd
    run_rawcooked --check sequence1
    check_success "check failed on valid dpx sequence" "check succeded on valid dpx sequence"

    mkdir sequence2
    ls sequence1 >&$fd
    ffmpeg -f lavfi -i testsrc=duration=0.200:size=16x16 -start_number 9 sequence2/%01d.dpx >&$fd
    run_rawcooked --check sequence2
    check_failure "check failed due to incomplete dpx sequence" "check succeded despite incomplete dpx sequence"

    clean
popd >/dev/null 2>&1

exit ${status}
