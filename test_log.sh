#! /usr/bin/env bash
set -e # exit on error

testfile=logtest
./test_log ${testfile}
ls -l ${testfile}.*
cmp ${testfile}.string.write ${testfile}.string.mmap
cmp ${testfile}.ints.write ${testfile}.ints.mmap
rm ${testfile}*.write ${testfile}*.mmap
