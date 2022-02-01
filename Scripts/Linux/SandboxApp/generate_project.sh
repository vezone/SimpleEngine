#!/bin/bash

cd ../../..
premake5 --file="premake-sandbox-app.lua" --cc=clang gmake2
