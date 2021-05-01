#!/bin/bash

cd ../../..
valgrind --leak-check=yes --show-leak-kinds=definite -s  bin/Debug-linux-x86_64/SimpleTest/SimpleTest
