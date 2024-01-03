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

	// 读取文件。
	unsigned int num = argc;
	char **filenames = ++argv;
	ReadElf(num, filenames);
	// 收集信息。
	CollectInfo();
	// 分配地址空间。
	// unsigned int base = 0x8048000;
	unsigned int base = 0x08048000 + 52 + sizeof(Elf32_Phdr) * 3;
	AllocAddress(&base);
	// 解析符号。
	ParseSym();
	// 重定位。
	Relocation();
	// 组装ELF文件。
	ELF32 elf32 = AssembleELF();
	// 写入文件。
	WriteElf(elf32);
	
	// 合并段。
//	MergeSegment();

	return 0;
}
