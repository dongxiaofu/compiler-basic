#include "linker.h"

void PrintStrtabTest(char *strtab, unsigned int size)
{
	unsigned int len = 0;

	printf("\n--------printStrtab start-------\n");
	while(len < size){
		char *str = strtab;
		printf("str = %s\n", str);

		len += strlen(str) + 1;
		strtab += strlen(str) + 1;
	}
	printf("\n--------printStrtab end-------\n");
}

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
	// 找到所有已经定义的符号。
	Node strtabLinkList = NULL;
	Node preHead = NULL;
	Node localSymLinkList = InitLinkList();
	Node globalSymLinkList = InitLinkList();
	ELF32 elf32 = elf32LinkList->next;
	int i = 0;
	while(elf32 != NULL){
		printf("elf32--%d\n", i++);
		Node head = ExplodeStrtab(elf32->strtab->addr, elf32->strtab->size);	
		if(strtabLinkList == NULL){
			strtabLinkList = head;
		}else{
			strtabLinkList = MergeLinkList(strtabLinkList, head);
		}

		// 处理.symtab：
		// 1. 把.symtab分成local和global两类。
		// 2. 查询符合的name。
		Elf32_Sym *sym = (Elf32_Sym *)elf32->symtab->addr;
		unsigned int symSize = sizeof(Elf32_Sym);
		unsigned int symtabSize = 0;
		while(symtabSize < elf32->symtab->size){
  			unsigned char st_info = sym->st_info;		/* Symbol type and binding */
			// unsigned char bind = st_info >> 4;
			unsigned char bind = ELF32_ST_BIND(st_info);
			// unsigned char type = st_info & 0xF;
			unsigned char type = ELF32_ST_TYPE(st_info);

			Node node = (Node)MALLOC(sizeof(struct node));
			unsigned int symLinkSize = sizeof(struct symbolLink);
			SymbolLink symLink = (SymbolLink)MALLOC(symLinkSize);			
			symLink->sym = sym;
			symLink->name = elf32->strtab->addr + sym->st_name;
			printf("name = %s, st_name = %d\n", symLink->name, sym->st_name);
			node->val.symLink = symLink;
			node->type = SYMBOL_LINK;

			if(bind == STB_LOCAL){
				AppendNode(localSymLinkList, node);
			}else{
				AppendNode(globalSymLinkList, node);
			}

			symtabSize += symSize;
			sym++;
		}

		elf32 = elf32->next;
	}

	// 合并localSymLinkList和globalSymLinkList。
	Node allSymLinkList = MergeLinkList(localSymLinkList, globalSymLinkList);
	// 去掉st_name是0或未定义的符号。
	Node trimSymLinkList = InitLinkList();
	Node currentSymLinkNode = allSymLinkList->next;
	Node trimStrtabLinkList = InitLinkList();
	unsigned int trimStrtabSize = 0;
	while(currentSymLinkNode != allSymLinkList){
		SymbolLink symLink = currentSymLinkNode->val.symLink;
		printf("symLink name = %s\n", symLink->name);
		Elf32_Sym *sym = symLink->sym; 
		unsigned int shndx = sym->st_shndx; 
		unsigned int nameOffset = sym->st_name;
		// TODO 难点是这个条件。我不熟悉相关情况，所以不确定目前的条件是否正确。
		// 这个条件没有问题。
		// if(shndx != SHN_UNDEF || nameOffset != 0){
		if(shndx != SHN_UNDEF && nameOffset != 0){
			unsigned int nodeSize = sizeof(struct node);
			Node node = (Node)MALLOC(nodeSize);
			memcpy(node, currentSymLinkNode, nodeSize); 
			AppendNode(trimSymLinkList, node);
			Node strNode = (Node)MALLOC(sizeof(struct node));
			unsigned int strSize = strlen(symLink->name) + 1;
			trimStrtabSize += strSize;
			strNode->val.str = (char *)MALLOC(strSize);
			memcpy(strNode->val.str, symLink->name, strSize - 1);
			printf("strNode->val.str = %s\n", strNode->val.str);
			AppendNode(trimStrtabLinkList, strNode);
		}

		currentSymLinkNode = currentSymLinkNode->next;
	}

	char *trimStrtab = (char *)MALLOC(trimStrtabSize);
	char *trimStrtabStart = trimStrtab;
	unsigned int strtabOffset = 0;
	Node currentTrimStrtabNode = trimStrtabLinkList->next;
	while(currentTrimStrtabNode != trimStrtabLinkList){
		unsigned int len = strlen(currentTrimStrtabNode->val.str);
		memcpy(trimStrtab, currentTrimStrtabNode->val.str, len);
		trimStrtab += len + 1;

		currentTrimStrtabNode = currentTrimStrtabNode->next; 
	}

	PrintStrtabTest(trimStrtabStart, trimStrtabSize);
	
	// 合并段。
	MergeSegment();

	return 0;
}
