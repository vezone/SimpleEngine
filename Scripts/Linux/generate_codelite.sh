#!/bin/bash

cd ../../..
premake5 --file=$1 --cc=gcc codelite
