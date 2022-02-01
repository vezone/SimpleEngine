#!/bin/bash

cd ../../

premake5 --file=$2 --cc=gcc codelite

cp -R assets $1/
cp -R resources $1/

codelite Engine.workspace
