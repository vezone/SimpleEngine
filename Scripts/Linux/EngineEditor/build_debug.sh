#!/bin/bash

cd ../../..

startTime=$(date +%s%3N)
make config=debug
Scripts/Linux/copy_files.sh "EngineEditor"
endTime=$(date +%s%3N)
diffTime=$(( $endTime - $startTime ))

echo "Builds in $diffTime ms"

pwd
bin/Debug-linux-x86_64/EngineEditor/EngineEditor
