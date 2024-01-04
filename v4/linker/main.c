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

	entryFunctionName = NULL;
	char *defaultExecutableFileName = "a";
	executableFileName = defaultExecutableFileName;

	unsigned int fileNum = 0;
	unsigned int optionNum = 0;

	// link -e main -o ab a.o b.o c.o
	// 解析参数。
	for(unsigned int i = 1; i < argc; i++){
		char *argument = argv[i];
		printf("argument = %s\n", argument);
		if(*argument != '-'){
			// 处理完了所有的选项。
			break;
		}
		optionNum++;
		// 设置函数的入口。
		if(strcmp("-e", argument) == 0){
			i++;
			if(i >= argc){
				// TODO 讨厌这种处理方式。找机会优化。
				break;
			}
			optionNum++;
			argument = argv[i];
			unsigned int size = strlen(argument) + 1;
			entryFunctionName = (char *)MALLOC(size);
			memcpy(entryFunctionName, argument, size);	

		}else if(strcmp("-o", argument) == 0){
			i++;
			if(i >= argc){
				// TODO 讨厌这种处理方式。找机会优化。
				break;
			}
			optionNum++;
			argument = argv[i];
			unsigned int size = strlen(argument) + 1;
			executableFileName = (char *)MALLOC(size);
			memcpy(executableFileName, argument, size);
		}
	}

	printf("executableFileName = %s, entryFunctionName = %s, optionNum = %d, argc = %d\n", \
		executableFileName, entryFunctionName, optionNum, argc);

	// 读取文件。
	unsigned int num = argc - optionNum - 1;
	// char **filenames = ++argv;
	char **filenames = argv + 1 + optionNum;
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
