#/bin/bash

make
echo $1
echo $2
./linker $1 $2 $3
