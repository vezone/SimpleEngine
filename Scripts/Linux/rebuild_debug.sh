#!/bin/bash

./clean_all.sh
./generate_project.sh $2
./build_debug.sh $1
