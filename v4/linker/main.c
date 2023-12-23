#include "linker.h"

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
		
		

		// 导致错误。
		// int size = fread(elf, 1, 52, file);
		int size = fread(ehdr, 52, 1, file);
		if(size == -1){
			printf("read %s error\n", name);
			exit(-1);
		}

	 	// int fseek(FILE *stream, long offset, int whence);
	 	Elf32_Phdr *phdr = NULL;
	 	unsigned int phnum = ehdr->e_phnum;
		if(phnum > 0){
	 		int offset = fseek(file, ehdr->e_phoff, SEEK_SET);
			int phdrSize = ehdr->e_phentsize;
			phdr = (Elf32_Phdr *)MALLOC(sizeof(Elf32_Phdr));
			fread(phdr, phdrSize, 1, file);
		}

		Elf32_Shdr *shdr = NULL;
		unsigned int shnum = ehdr->e_shnum;
		if(shnum > 0){
			int offset = fseek(file, ehdr->e_shoff, SEEK_SET);
			int size = ehdr->e_shentsize;
			shdr = (Elf32_Shdr *)MALLOC(size * shnum);
			int bytes = fread(shdr, size, shnum, file);
		}
//	// .text
		char *strtab = NULL;
		char *shstrtab = NULL;
		unsigned int segmentSize = sizeof(struct segment);
		Elf32_Shdr *ptrShdr = shdr;
		for(int i = 0; i < shnum; i++){
			Elf32_Shdr *shdrEntry = ptrShdr++; 	
			printf("sh_offset = %d\n", shdrEntry->sh_offset);
			int size = shdrEntry->sh_size;
			int offset = fseek(file, shdrEntry->sh_offset, SEEK_SET);
			char *str = (char *)MALLOC(size);
			int bytes = fread(str, size, 1, file);

			Elf32_Word type = shdrEntry->sh_type;
			if(type == SHT_STRTAB){
				// GetSubStrIndex(char *subStr, char *str, unsigned int strLength)
				if(GetSubStrIndex(".text", str, size) != -1){
					// 是.shstrtab
					elf32->shstrtab = (Segment)MALLOC(segmentSize);
					elf32->shstrtab->addr = (void *)str;
					elf32->shstrtab->size = size;
					shstrtab = str;
				}else{
					// 是.strtab
					elf32->strtab = (Segment)MALLOC(segmentSize);
					elf32->strtab->addr = (void *)str;
					elf32->strtab->size = size;
					strtab = str;

					strtabSize += size;
				}
			}

			if(strtab && shstrtab){
				break;
			}
		}

		ptrShdr = shdr;
		for(int i = 0; i < shnum; i++){
			Elf32_Shdr *shdrEntry = ptrShdr++; 	
  			Elf32_Word	name = shdrEntry->sh_name;		
			char *subStr = shstrtab + name;
			printf("seg name = %s, sh_offset = %x, size = %d\n", subStr,\
				 shdrEntry->sh_offset, shdrEntry->sh_size);
			int size = shdrEntry->sh_size;
			int offset = fseek(file, shdrEntry->sh_offset, SEEK_SET);

			if(strcmp(subStr, ".text") == 0){
				// char *text = (char *)MALLOC(size);
				void *text = (void *)MALLOC(size);
				int bytes = fread(text, size, 1, file);
				elf32->text = (Segment)MALLOC(segmentSize);
				elf32->text->addr = text;
				// TODO 我不确定用bytes还是size。正常情况下，二者是相等的。
				elf32->text->size = size;

				textSize += size;

			}else if(strcmp(subStr, ".data") == 0){
				void *addr = (void *)MALLOC(size);
				int bytes = fread(addr, size, 1, file);
				elf32->data = (Segment)MALLOC(segmentSize);
				elf32->data->addr = (void *) addr;
				elf32->data->size = size;

				dataSize += size;

			}else if(strcmp(subStr, ".rel.data") == 0){
				Elf32_Rel *rel = (Elf32_Rel *)MALLOC(size);
				int bytes = fread(rel, size, 1, file);
				elf32->relData = (Segment)MALLOC(segmentSize);
				elf32->relData->addr = (void *)rel;
				elf32->relData->size = size;

				relDataSize += size;

			}else if(strcmp(subStr, ".rel.text") == 0){
				Elf32_Rel *rel = (Elf32_Rel *)MALLOC(size);
				int bytes = fread(rel, size, 1, file);
				elf32->relText = (Segment)MALLOC(segmentSize);
				elf32->relText->addr = (void *)rel;
			    elf32->relText->size = size;

				relTextSize += size;

			}else if(strcmp(subStr, ".rodata") == 0){
				void *addr = (void *)MALLOC(size);
				int bytes = fread(addr, size, 1, file);
				elf32->rodata = (Segment)MALLOC(segmentSize);
				elf32->rodata->addr = (void *) addr;
				elf32->rodata->size = size;

				rodataSize += size;

			}else if(strcmp(subStr, ".symtab") == 0){
				void *addr = (void *)MALLOC(size);
				int bytes = fread(addr, size, 1, file);
				elf32->symtab = (Segment)MALLOC(segmentSize);
				elf32->symtab->addr = (void *) addr;
				elf32->symtab->size = size;

			}else{
				// TODO 怎么处理？
			}
		}


		elf32->ehdr = ehdr;
		elf32->phdr = phdr;
		elf32->shdr = shdr;

		AppendElf32LinkList(elf32);
	}

	printf("over\n");

	// 合并段。
	MergeSegment();

	return 0;
}
