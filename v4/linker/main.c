#include "linker.h"

int main(int argc, char *argv[])
{
	printf("hi, I am a linker.\n");

	if(argc < 2){
		printf("require a elf file\n");
		exit(-1);
	}


	unsigned int num = argc;
//	elf32LinkList = (ELF32)MALLOC(sizeof(struct elf32));
	
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

		ELF32 elf32 = (ELF32)MALLOC(sizeof(struct elf32));

		Elf32_Ehdr *ehdr = (Elf32_Ehdr *)MALLOC(52);;
//		elf32->ehdr = ehdr;
		
		

		// 导致错误。
		// int size = fread(elf, 1, 52, file);
		int size = fread(ehdr, 52, 1, file);
		if(size == -1){
			printf("read %s error\n", name);
			exit(-1);
		}



		elf32->ehdr = ehdr;

		AppendElf32LinkList(elf32);

	}

		printf("over\n");

	return 0;
}
