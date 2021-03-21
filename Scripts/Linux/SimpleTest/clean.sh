#!/bin/bash

cd ../../..
rm Makefile
rm Engine.workspace
rm tags
rm -rf bin
rm -rf .codelite
rm -rf .build-debug

rm Engine/Makefile
rm Engine/compile_flags.txt
rm Engine/Engine.mk
rm Engine/Engine.project
rm Engine/Engine.txt

rm EngineEditor/Makefile
rm EngineEditor/compile_flags.txt
rm EngineEditor/EngineEditor.mk
rm EngineEditor/EngineEditor.project
rm EngineEditor/EngineEditor.txt

rm SimpleTest/Makefile
rm SimpleTest/compile_flags.txt
rm SimpleTest/SimpleTest.mk
rm SimpleTest/SimpleTest.project
rm SimpleTest/SimpleTest.txt

rm SandboxApp/Makefile
rm SandboxApp/compile_flags.txt
rm SandboxApp/SandboxApp.mk
rm SandboxApp/SandboxApp.project
rm SandboxApp/SandboxApp.txt

rm -rf Dependencies/cimgui/bin
rm Dependencies/cimgui/cimgui.mk
rm Dependencies/cimgui/cimgui.project
rm Dependencies/cimgui/cimgui.txt
rm Dependencies/cimgui/compile_flags.txt
rm Dependencies/cimgui/Makefile
rm Dependencies/cimgui/cimgui.project

rm -rf Dependencies/glad/bin
rm Dependencies/glad/glad.mk
rm Dependencies/glad/glad.project
rm Dependencies/glad/glad.txt
rm Dependencies/glad/compile_flags.txt
rm Dependencies/glad/Makefile
rm Dependencies/glad/glad.project

rm -rf Dependencies/GLFW/bin
rm Dependencies/GLFW/GLFW.mk
rm Dependencies/GLFW/GLFW.project
rm Dependencies/GLFW/GLFW.txt
rm Dependencies/GLFW/compile_flags.txt
rm Dependencies/GLFW/Makefile
rm Dependencies/GLFW/GLFW.project
