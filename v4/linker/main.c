#include "linker.h"

// void PrintStrtabTest(char *strtab, unsigned int size)
// {
// 	unsigned int len = 0;
// 
// 	printf("\n--------printStrtab start-------\n");
// 	while(len < size){
// 		char *str = strtab;
// 		printf("str = %s\n", str);
// 
// 		len += strlen(str) + 1;
// 		strtab += strlen(str) + 1;
// 	}
// 	printf("\n--------printStrtab end-------\n");
// }

int main(int argc, char *argv[])
{
	printf("hi, I am a linker.\n");

	if(argc < 2){
		printf("require a elf file\n");
		exit(-1);
	}

	textSize = 0;
	dataSize = 0;
	rodataSize = 0;
	relDataSize = 0;
	relTextSize = 0;
	symtabSize = 0;
	strtabSize = 0;

	unsigned int num = argc;
	char **filenames = ++argv;
	CollectInfo(num, filenames);
	// 
	// 合并所有的`.strtab`。
	
	// 合并段。
	MergeSegment();

	return 0;
}
