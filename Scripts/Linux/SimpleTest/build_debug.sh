#!/bin/bash

cd ../../..
make config=debug
Scripts/Linux/copy_files.sh "SimpleTest"
bin/Debug-linux-x86_64/SimpleTest/SimpleTest
