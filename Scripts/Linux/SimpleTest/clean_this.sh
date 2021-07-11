#!/bin/bash

cd ../../..

#files[0]="Makefile"
#files[1]="Engine.workspace"
#files[2]="tags"

#projectsDirs[0]="Engine"
#projectsDirs[2]="SimpleTest"

binDirs[0]="bin"
binDirs[1]=".codelite"
binDirs[2]=".build-debug"

for file in ${files[*]}
do
    if [ -e $file ]; then
	rm $file
    fi
done

for projectDir in ${projectsDirs[*]}
do
    if [[ -d $projectDir && -e "${projectDir}/Makefile" ]]; then
	rm -rf "${projectDir}/assets"
	rm "${projectDir}/${projectDir}.mk"
	rm "${projectDir}/${projectDir}.project"
	rm "${projectDir}/${projectDir}.txt"
	rm "${projectDir}/imgui.ini"
	rm "${projectDir}/compile_flags.txt"
	rm "${projectDir}/Makefile"
    fi
done

for binDir in ${binDirs[*]}
do
    if [ -d $binDir ]; then
	rm -rf $binDir
    fi
done
