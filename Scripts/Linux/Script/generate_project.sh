 #!/bin/bash

cd ../../..
premake5 --file="premake-build-shared-lib.lua" --cc=clang gmake2
