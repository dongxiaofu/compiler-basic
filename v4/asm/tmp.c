#include <stdio.h>

int main(int argc, char *argv[])
{
	int num = 89;
	unsigned char ch[4];
	// ch[0] = (0xFF | num);
	ch[0] = (0xFF & num);


	return 0;
}
