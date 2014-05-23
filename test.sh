#!/bin/bash
cp malloc.c testing/
cp malloc.h testing/
cp brk.h testing/
cd testing/
make clean
make
bash RUN_TESTS
