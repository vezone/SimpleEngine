#!/bin/bash

cd ../..
make config=release
Scripts/Linux/copy_files.sh "$1"
bin/Release-linux-x86_64/$1/$1
