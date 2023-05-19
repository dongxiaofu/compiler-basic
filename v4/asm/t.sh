#! /bin/bash 

ucc -E t1.c -o t1.i
ucc --dump-ast --dump-IR -S t1.i -o test.s
