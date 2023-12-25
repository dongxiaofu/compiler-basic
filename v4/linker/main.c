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
	char **filenames = ++argv;
	CollectInfo(num, filenames);
	// 合并所有的`.strtab`。
	// 找到所有已经定义的符号。
	Node strtabLinkList = NULL;
	Node preHead = NULL;
	ELF32 elf32 = elf32LinkList->next;
	while(elf32 != NULL){
		Node head = ExplodeStrtab(elf32->strtab->addr, elf32->strtab->size);	
		if(strtabLinkList == NULL){
			strtabLinkList = head;
		}else{
			strtabLinkList = MergeLinkList(strtabLinkList, head);
		}

		elf32 = elf32->next;
	}
	// 合并段。
	MergeSegment();

	return 0;
}
