#!/bin/bash

cd ../..

files[0]="Makefile"
files[1]="Engine.workspace"
files[2]="tags"

projectsDirs[0]="Engine"
projectsDirs[1]="SimpleEditor"
projectsDirs[2]="SimpleTest"
projectsDirs[3]="SandboxApp"

dependenciesDirs[0]="cimgui"
dependenciesDirs[1]="glad"
dependenciesDirs[2]="GLFW"

binDirs[0]="bin"
binDirs[1]=".codelite"
binDirs[2]=".build-debug"

function file_delete_if_exist() {
    if [ -e $1 ]; then
	rm $1
    fi
}

function dir_delete_if_exist() {
    if [[ -d $1 ]]; then
	rm -rf $1
    fi

}

for file in ${files[*]}
do
    file_delete_if_exist $file
done

for projectDir in ${projectsDirs[*]}
do
    #if [[ -d $projectDir && -e "${projectDir}/Makefile" ]]; then
    #	rm -rf "${projectDir}/"
    #fi
    dir_delete_if_exist "${projectDir}/assets"
    dir_delete_if_exist "${projectDir}/resources"

    file_delete_if_exist "${projectDir}/${projectDir}.mk"
    file_delete_if_exist "${projectDir}/${projectDir}.project"
    file_delete_if_exist "${projectDir}/${projectDir}.txt"
    file_delete_if_exist "${projectDir}/imgui.ini"
    file_delete_if_exist "${projectDir}/compile_flags.txt"
    file_delete_if_exist "${projectDir}/Makefile"

done

for dependencyDir in ${dependenciesDirs[*]}
do
    dir="Dependencies/${dependencyDir}"
    dir_delete_if_exist "$dir/bin"
    file_delete_if_exist "$dir/${dependencyDir}.mk"
    file_delete_if_exist "$dir/${dependencyDir}.project"
    file_delete_if_exist "$dir/${dependencyDir}.txt"
    file_delete_if_exist "$dir/compile_flags.txt"
    file_delete_if_exist "$dir/Makefile"
done

for binDir in ${binDirs[*]}
do
    dir_delete_if_exist $binDir
done
