#!/bin/bash

cd ../..

startTime=$(date +%s%3N)
make config=debug
Scripts/Linux/copy_files.sh "$1"
endTime=$(date +%s%3N)
diffTime=$(( $endTime - $startTime ))

echo "Builds $1 in $diffTime ms"

pwd
bin/Debug-linux-x86_64/$1/$1
