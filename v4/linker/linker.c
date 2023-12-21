#include "linker.h"

void AppendElf32LinkList(ELF32 node)
{
	if(elf32LinkList == NULL){
		elf32LinkList = (ELF32)MALLOC(sizeof(struct elf32));
		elf32LinkList->next = node;
	}else{
		preElf32->next = node;	
	}

	preElf32 = node;
}
