#! /usr/bin/env bash
set -e # exit on error

cat > testdata <<EOF
This is 
a testfile 
for testing logfile
EOF

./test_logger logfile.output < testdata
diff logfile.output testdata
