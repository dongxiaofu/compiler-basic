#include <stdio.h>
#include <string.h>

int sum(int a, int b);

// [root@localhost my-demo]# gcc -S ch.s ch.c -m32
// ch.c:6:12: error: initializer element is not constant
// int num2 = sum(4,5);
//            ^~~
// int num2 = sum(4,5);
static char ch7;
char ch4;
char *str[2] = {"hello", "world"};
int num33,num34;
double num13[3] = {4.9,5.2,3.7};
float num11 = 4.1;
double num12 = 5.8;
char ch1 = 'A';
const char ch2 = 'B';
int num1 = 14;
char *my_str_ak = "How are you Jim";
int num2 = 140;
char *my_str_cg = "How are you";
/***
 * ch.c:17:17: error: initializer element is not constant
 char *my_str2 = my_str_cg;
                 ^~~~~~~~~
 */
// char *my_str2 = my_str_cg;


int main(int argc, char *argv[])
{
	int x = sum(5,9);
	const char ch38 = 'D';
	static char ch44 = 'E';
	static char ch55;
	static int num66;
	printf("%d", num1);
	printf(my_str_cg);
	printf(my_str_ak);
	// char *str = "自己创建文件";
	char *str = "输入盘子个数：";
	int len = strlen(str);
	int len2 = strlen(my_str_cg);
	char *str2 = my_str_cg;
	
	for(int i = 0; i < len; i++){
		// unsigned char ch = str[i];
		char ch_cg = str[i];
		printf("cx = %d\n", ch_cg);
	}

	printf("@---------next--------\n");

	int cnt = 0;
	int cont[255] = {0};
	for(int i = 0;; i++){
		if(str[i] == '\0')	break;
		cont[i] = str[i];
		printf("str[%d] = %c\n", i, str[i]);
	}

	return 0;
}


int sum(int a, int b)
{
	return a+b;
}
