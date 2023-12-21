#include "common.h"

int main(int argc, char *argv[])
{
	printf("hi, I am a linker.\n");

	if(argc < 2){
		printf("require a elf file\n");
		exit(-1);
	}


	unsigned int num = argc;
	
	for(int i = 1; i < num; i++){
		char *name = argv[i];
		printf("read file %s\n", name);
		char path[20] = "obj/";
		strcat(path, name);

		FILE *file = fopen(path, "rb");
		if(file == NULL){
			printf("open %s error\n", path);
			exit(-1);
		}

		Elf32_Ehdr *elf = (Elf32_Ehdr *)MALLOC(52);;

		// 导致错误。
		// int size = fread(elf, 1, 52, file);
		int size = fread(elf, 52, 1, file);
		if(size == -1){
			printf("read %s error\n", name);
			exit(-1);
		}

		printf("over\n");


	}


	return 0;
}
