#!/bin/bash

cd ../../..
make config=release
Scripts/Linux/copy_files.sh "SimpleTest"
bin/Release-linux-x86_64/SimpleTest/SimpleTest
