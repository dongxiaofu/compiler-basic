echo $1

gcc -S $1.c -o $1.s  -m32
gcc -c $1.s -o $1.o -m32

if [ $2 == 1 ]
then
	gcc $1.c -o $1 -g -m32
fi
#gcc -S ch.c -o ch.s  -m32
#gcc -c ch.s -o ch.o -m32
