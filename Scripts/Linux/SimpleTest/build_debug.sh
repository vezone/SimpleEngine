#!/bin/bash

cd ../../..

startTime=$(date -Ins)

make config=debug
Scripts/Linux/copy_files.sh "SimpleTest"

endTime=$(date -Ins)
diffTime=$(( $endTime - $startTime ))

echo "Build takes $diffTime seconds"

pwd
bin/Debug-linux-x86_64/SimpleTest/SimpleTest
