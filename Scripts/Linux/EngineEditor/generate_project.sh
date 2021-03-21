#!/bin/bash

cd ../../..

cd Dependencies/cimgui/cimgui/src/generator/
sh ./generator.sh
cd ../../../../../
premake5 --file="premake5.lua" --cc=gcc gmake2
