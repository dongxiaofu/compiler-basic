#include <stdio.h>

int main(int argc, char *argv[])
{
	const volatile int a = 5;
	printf("a = %d\n", a);

	return 0;
}
