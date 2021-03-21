#!/bin/bash

cd ../../..
premake5 --file="premake-simple-test.lua" --cc=clang gmake2
