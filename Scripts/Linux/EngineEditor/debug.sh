#!/bin/bash

cp -R ../../../assets bin/Debug-linux-x86_64/EngineEditor/
./generate_codelite.sh
codelite ../../../Engine.workspace
