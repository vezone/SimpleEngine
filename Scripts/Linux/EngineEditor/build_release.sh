#!/bin/bash

cd ../../..
make config=release
Scripts/Linux/copy_files.sh "EngineEditor"
bin/Release-linux-x86_64/EngineEditor/EngineEditor
