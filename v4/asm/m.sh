#! /bin/bash


#ucc -E t1.c -o t1.i
#ucc --dump-ast --dump-IR -S t1.i -o test.s

make clean
make
/home/cg/code/compiler/vm/cg-asm/cg_assembler test.s > ./log
cp -rvf ./test.cxe ../cvm/
