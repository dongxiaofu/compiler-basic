//#include <stdio.h>

extern int shared;

int main()
{
	int a = 100;

	swap(&a, &shared);

	printf("a = %d\n", a);

	return 0;
}
