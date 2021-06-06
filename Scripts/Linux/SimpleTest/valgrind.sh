#!/bin/bash

cd ../../..
valgrind --leak-check=yes --show-leak-kinds=definite --track-origins=yes -s  bin/Debug-linux-x86_64/SimpleTest/SimpleTest
