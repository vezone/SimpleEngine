#!/bin/bash


projectReleasePath="bin/Release-linux-x86_64/$1/"
projectDebugPath="bin/Debug-linux-x86_64/$1/"

if [ -d $projectReleasePath ]; then
    cp -R assets "${projectReleasePath}"
    cp -R imgui.ini "${projectReleasePath}imgui.ini"
    cp -R resources "${projectReleasePath}"
fi

if [ -d $projectDebugPath ]; then
    cp -R assets $projectDebugPath
    cp -R imgui.ini "${projectDebugPath}imgui.ini"
    cp -R resources "${projectReleasePath}"
fi

cp -R assets "${1}"
cp imgui.ini "${1}/imgui.ini"
