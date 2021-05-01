#!/bin/bash

cd ../../..
make config=debug
Scripts/Linux/copy_files.sh "EngineEditor"
bin/Debug-linux-x86_64/EngineEditor/EngineEditor
