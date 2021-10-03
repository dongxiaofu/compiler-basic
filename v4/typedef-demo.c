#include <stdio.h>

int arr[3][4];
typedef int (*ArrPtr)[4];

ArrPtr ptr = &arr[0];

int main(int argc, char *argv[])
{
	**arr = 1;
	printf("ptr = %d\n", *ptr);
	**ptr = 2;
	printf("ptr = %d\n", *ptr);

	return 0;
}
