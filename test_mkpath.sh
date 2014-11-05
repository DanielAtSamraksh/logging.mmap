#! /usr/bin/env bash
set -e # exit on error

for testdir in _testdir _testdir/t1 _testdir/t1/t2/t3 _testdir/t2/t3; do
    echo "testing with ${testdir}"
    if ls ${testdir} 2> /dev/null; then rm -r ${testdir}; fi
    ./test_mkpath ${testdir}
    ls ${testdir}
    rm -r ${testdir}
    echo "${testdir} is OK"; echo
done
