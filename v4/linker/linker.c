#include "linker.h"

Node ExplodeStrtab(char *strtab, unsigned int size)
{
	Node head = InitLinkList();
	unsigned int len = 0;
	while(len < size){
		char *str = strtab + len;
		unsigned int strLen = strlen(str) + 1;
		
		Node node = (Node)MALLOC(sizeof(struct node)); 
		node->type = STRING;
		node->val.str = (char *)MALLOC(strLen); 
		memcpy(node->val.str, str, strLen);
		
		AppendNode(head, node);

		len += strLen;
	}

	return head;
}

Segment UniqueLinkListStr(Node list)
{
	Node currentNode = list->next;
	int size = 1;
	Node head = InitLinkList();

	while(currentNode->next != list){
		char *str = currentNode->val.str;
		if(*str == '\000'){
			currentNode = currentNode->next;
			continue;
		}

		if(FindNodeByStr(head, str) == NULL){
			size += strlen(str) + 1;
			unsigned int nodeSize = sizeof(struct node);
			// 必须新建节点，不能直接使用currentNode。
			Node node = (Node)MALLOC(nodeSize);
			memcpy(node, currentNode, nodeSize);
			AppendNode(head, node);
		}

		currentNode = currentNode->next;
	}	

	Segment segment = (Segment)MALLOC(sizeof(struct segment));
//	segment->addr = (char *)MALLOC(size);
	char *addr = (char *)MALLOC(size);
	char *addrStart = addr;
	addr++;
	currentNode = head->next;
	while(currentNode != head){
		char *str = currentNode->val.str;
		printf("str = %s in UniqueLinkListStr\n", str);
		unsigned int len = strlen(str);
		memcpy(addr, str, len);
		printf("addr = %s in UniqueLinkListStr\n", addr);
		addr += len + 1; 

		currentNode = currentNode->next;
	}
	
	segment->addr = addrStart;
	segment->size = size;

	return segment;
}

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

SegNameSegTabEntry FindSegNameSegTabEntryByIndex(Node segTabLinkList, unsigned int index)
{
	SegNameSegTabEntry target = NULL;

	Node node = segTabLinkList->next;
	while(node != segTabLinkList){
		SegNameSegTabEntry entry = node->val.segTab;
		if(entry->index == index){
			target = entry;
			break;
		}
		node = node->next;
	}

	return target;
}

SegNameSegTabEntry FindSegNameSegTabEntryByName(Node segTabLinkList, char *segName)
{
	SegNameSegTabEntry target = NULL;

	Node node = segTabLinkList->next;
	while(node != segTabLinkList){
		SegNameSegTabEntry entry = node->val.segTab;
		unsigned int len = strlen(segName);
		// if(strcmp(segName, entry->segName) == 0){
		if(strncmp(segName, entry->segName, len) == 0){
			target = entry;
			break;
		}
		node = node->next;
	}

	return target;
}

Elf32_Ehdr *GenerateELFHeader()
{
	// ELF文件头
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)MALLOC(sizeof(Elf32_Ehdr));

	ehdr->e_ident[0] = 0x7F;
	ehdr->e_ident[1] = 0x45;
	ehdr->e_ident[2] = 0x4C;

	ehdr->e_ident[3] = 0x46;
	ehdr->e_ident[4] = 1;
	ehdr->e_ident[5] = 1;
	ehdr->e_ident[6] = 1;

	for(int i = 7; i < 16; i++){
		ehdr->e_ident[i] = 0;
	}

	ehdr->e_type =	ET_EXEC; 	
	ehdr->e_machine = EM_386;
	ehdr->e_version = 1;
	ehdr->e_entry = 0x08048000;
	ehdr->e_phoff = 52;
	// TODO 需计算。
	// 段表在文件中的偏移。
	ehdr->e_shoff = 0;
	// TODO 不知道设置成什么值。
	ehdr->e_flags = 0;
	// 是一个固定值。
	ehdr->e_ehsize = 52;
	ehdr->e_phentsize = sizeof(Elf32_Phdr);
	ehdr->e_phnum = 3;
	ehdr->e_shentsize = 40;
	// TODO 段表条目的数量，待定。
	ehdr->e_shnum = 7;
	// TODO .shstrtab在段表中的索引。
	// ehdr->e_shstrndx = 0;
	ehdr->e_shstrndx = 6;

	return ehdr;
}

unsigned int RoundUp(unsigned int num, unsigned int base)
{
	unsigned int quotient = (num + base - 1) / base;
	unsigned int result = quotient * base;

	return result;
}

// 为一个段分配地址空间。
// 这是一个简化之后的函数。
void AllocSegmentAddress(char *segName, unsigned int *base, unsigned int *offset, Node segList)
{
	ELF32 currentElf32 = elf32LinkList->next;
	
	unsigned int size = 0;

	while(currentElf32 != NULL){
		Segment segment = NULL;
		Node segTabLinkList = currentElf32->segTabLinkList;
		SegNameSegTabEntry entry = FindSegNameSegTabEntryByName(segTabLinkList, segName);
		if(entry == NULL){
			printf("error! seg %s is not exists in %d in %s\n", segName, __LINE__, __FILE__);
			// exit(-1);
			currentElf32 = currentElf32->next;
			continue;
		}

		// 因为Node是指针，所以在更新entry前把Node存储到链表中。这就是指针的好处。
		Node node = (Node)MALLOC(sizeof(struct node));
		node->val.segTab = entry;
		node->type = NODE_VALUE_SEG_TAB;
		AppendNode(segList, node); 

		entry->offset = size;
		

		Elf32_Shdr *shdr = entry->shdr;
		shdr->sh_offset = *offset;
		
		// entry->addr = (unsigned int)shdr->sh_addr;
		// 上面这句有问题。
		if(strcmp(segName, ".text") == 0){
			entry->addr = (unsigned int)currentElf32->text->addr;;
		}else if(strcmp(segName, ".data") == 0){
			entry->addr = (unsigned int)currentElf32->data->addr;;
		}else if(strcmp(segName, ".rodata") == 0){
			entry->addr = (unsigned int)currentElf32->rodata->addr;;
		}else{
			// TODO 不会出现这种情况。
		}

		shdr->sh_addr = *base + size;
		printf("%s:%s sh_addr = %x, shdr->sh_size = %d\n", currentElf32->filename,\
			 segName, shdr->sh_addr, shdr->sh_size);
		size += shdr->sh_size;
		*offset += shdr->sh_size;

		currentElf32 = currentElf32->next;
	}

	*base += size;
}

void AllocAddress(unsigned int *base)
{
	char *segNames[3] = {".text", ".data", ".rodata"};
	unsigned int offset = 0;
	offset += 52;	// 文件头。
	offset += sizeof(Elf32_Phdr) * 3;	// 程序头。
	offset = RoundUp(offset, 0x1000);

	for(int i = 0; i < 3; i++){
		printf("base = %d\n", *base);
		segLists[i] = InitLinkList();
		char *segName = segNames[i];
		*base = RoundUp(*base, 0x1000);
		offset = RoundUp(offset, 0x1000);
		segOffsetLists[i] = offset;
		AllocSegmentAddress(segName, base, &offset, segLists[i]);
	}
}

// 收集信息。
// 什么信息？把符号分成定义的符号和未定义的符号。
void CollectInfo()
{
	ELF32 currentElf32 = elf32LinkList->next;
	symDefine = InitLinkList();
	symLink = InitLinkList();
	
	while(currentElf32 != NULL){
		Segment symSeg = currentElf32->symtab;
		if(symSeg == NULL){
			currentElf32 = currentElf32->next;
			continue;
		}

		char *strtab = NULL;
		Segment strtabSeg = currentElf32->strtab;
		if(strtabSeg == NULL){
			printf("strtab can't be NULL when symtab is not NULL in %d in %s", __LINE__, __FILE__);	
			exit(-1);
		}else{
			strtab = (char *)strtabSeg->addr;
		}

		unsigned int symbolLinkIndex = 0;
		unsigned int size = 0;
		Elf32_Sym *sym = (Elf32_Sym *)symSeg->addr;
		while(size < symSeg->size){
			SymbolLink symbolLink = (SymbolLink)MALLOC(sizeof(struct symbolLink));
			symbolLink->name = strtab + sym->st_name;
			symbolLink->sym = sym;
			symbolLink->index = symbolLinkIndex++;
			
			unsigned char bind = ELF32_ST_BIND(sym->st_info);
			// if(bind == STB_GLOBAL){
			// TODO 暂未明确是否收集local符号，所以只能这样做。
			if(1 == 1){
				Node node = (Node)MALLOC(sizeof(struct node));
				node->val.symLink = symbolLink;
				node->type = SYMBOL_LINK; 
				if(sym->st_shndx == SHN_UNDEF){
					symbolLink->provider = NULL;
					symbolLink->receiver = currentElf32;
					AppendNode(symLink, node);
				}else{
					symbolLink->provider = currentElf32;
					symbolLink->receiver = NULL;
					AppendNode(symDefine, node);
				}
			}

			sym++;
			size += sizeof(Elf32_Sym);
		}

		currentElf32 = currentElf32->next;
	}
}

void ReadElf(unsigned int num, char **filenames)
{
	for(int i = 1; i < num; i++){
		// char *name = filenames[i];
		// char *name = filenames++;
		char *name = (char *)(*filenames);
		filenames++;
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
		
		// 先读取.shstrtab和.strtab的目的是在下面获取段名称。
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

		// 格式化shdr。
		elf32->segTabLinkList = (void *)InitLinkList();
		Elf32_Shdr *shdrPtr = shdr;
		shstrtab = elf32->shstrtab->addr;
		for(int i = 0; i < shnum; i++){
			SegNameSegTabEntry entry = (SegNameSegTabEntry)MALLOC(sizeof(struct segNameSegTabEntry));
			entry->segName = shdrPtr->sh_name + shstrtab;
			printf("section name = %s\n", entry->segName);
			entry->shdr = shdrPtr;
			entry->index = i;
			entry->size = shdrPtr->sh_size;

			Node node = (Node)MALLOC(sizeof(struct node));
			node->val.segTab = entry;
			node->type = NODE_VALUE_SEG_TAB;
			AppendNode((Node)elf32->segTabLinkList, node);

			shdrPtr++;
		}


		elf32->filename = name;
		elf32->ehdr = ehdr;
		elf32->phdr = phdr;
		elf32->shdr = shdr;

		AppendElf32LinkList(elf32);
	}
}

SymbolLink FindSymbolLinkBySymName(Node list, char *symName)
{
	SymbolLink target = NULL;

	Node currentNode = list->next;
	while(currentNode != list){
		SymbolLink symLink = currentNode->val.symLink;
		if(strcmp(symName, currentNode->val.symLink->name) == 0){
			target = symLink; 
			break;
		}
		currentNode = currentNode->next;
	}

	return target;
}

SymbolLink FindSymbolLinkBySymIndex(Node list, unsigned int index)
{
	SymbolLink target = NULL;

	Node currentNode = list->next;
	while(currentNode != list){
		SymbolLink symLink = currentNode->val.symLink;
		if(symLink->index == index){
			target = symLink; 
			break;
		}
		currentNode = currentNode->next;
	}

	return target;
}

// 解析符号。
void ParseSym()
{
	// 处理已经定义的符号。
	Node currentSymDefineNode = symDefine->next;
	while(currentSymDefineNode != symDefine){
		SymbolLink symbolLink = currentSymDefineNode->val.symLink;
		ELF32 elf32 = symbolLink->provider;
		char *strtab = (char *)elf32->strtab->addr;
		Elf32_Sym *sym = symbolLink->sym;
		// TODO 没有想到更好的方法，只能这样做。
		if(sym->st_shndx == SHN_ABS){
			currentSymDefineNode = currentSymDefineNode->next;
			continue;
		}
		Node segTabLinkList = (Node)elf32->segTabLinkList;
		SegNameSegTabEntry segNameSegTabEntry = FindSegNameSegTabEntryByIndex(segTabLinkList, sym->st_shndx);
		Elf32_Shdr *shdr = segNameSegTabEntry->shdr; 
		if(shdr == NULL){
			printf("symbol %s has an error in %d in %s\n", symbolLink->name, __LINE__, __FILE__);
			exit(-1);
		}
		unsigned int old_value = sym->st_value;
		sym->st_value += shdr->sh_addr;
		printf("file = %s, sym name = %s, old_value = %d, st_value = %d, sh_addr = %d, segName = %s\n",\
			 elf32->filename, strtab + sym->st_name, old_value, sym->st_value,\
			 shdr->sh_addr, segNameSegTabEntry->segName);

		currentSymDefineNode = currentSymDefineNode->next;
	}

	// 处理引用的符号。
	Node currentSymLinkNode = symLink->next;
	while(currentSymLinkNode != symLink){
		SymbolLink symbolLink = currentSymLinkNode->val.symLink;
		char *name = symbolLink->name;
		// 不检查localSymDefine是否为NULL，在其他地方检查。
		SymbolLink localSymDefine = FindSymbolLinkBySymName(symDefine, name);
		symbolLink->sym->st_value = localSymDefine->sym->st_value;
		
		currentSymLinkNode = currentSymLinkNode->next;
	}
}

// 重定位。被调用的函数。
void RelocationAddr(ELF32 elf32, Elf32_Rel *rel, Elf32_Sym *sym, unsigned char type)
{
	Node segTabLinkList = elf32->segTabLinkList;
	void *segAddr = NULL;
	void *rodataSegAddr = NULL;
	void *segAddrInFile = NULL;
	if(type == 1){
		segAddr = elf32->data->addr;
		// ELF文件有.rel.data，必定有.rodata。
		rodataSegAddr = elf32->rodata->addr;
		SegNameSegTabEntry segTab= FindSegNameSegTabEntryByName(segTabLinkList, ".data");
		Elf32_Shdr *shdr = segTab->shdr;
		segAddrInFile = (void *)shdr->sh_addr;
	}else{
		segAddr = elf32->text->addr;
		SegNameSegTabEntry segTab= FindSegNameSegTabEntryByName(segTabLinkList, ".text");
		Elf32_Shdr *shdr = segTab->shdr;
		segAddrInFile = (void *)shdr->sh_addr;
	}
	int *ptrSymbol = NULL;
	// ptrSymbol = (unsigned int)segAddr + rel->r_offset;	
	ptrSymbol = (int *)((unsigned int)segAddr + rel->r_offset);	
	printf("before relocation, data = %d\n", *ptrSymbol);
	// 实际上上虚拟地址。
	int *ptrSymbolInFile = (int *)((unsigned int)segAddrInFile + rel->r_offset);	

	unsigned char relocationType = rel->r_info & 0xFF;
	if(relocationType == R_386_32){
		if(type == 1){
			*ptrSymbol = *(int *)((unsigned int)rodataSegAddr + (int )(*ptrSymbol));
		}else{
			// 处理完.rel.data后,再处理.rel.text时会执行到这里。
			*ptrSymbol = sym->st_value; 		
		}
	}else if(type == R_386_PC32){
		// 相对重定位，我依然不能在几秒钟想出计算方法。
		// *ptrSymbol = sym->st_value - ptrSymbol + *ptrSymbol;
		// *ptrSymbol = sym->st_value - (unsigned int)ptrSymbol + *ptrSymbol;
		*ptrSymbol = sym->st_value - (unsigned int)ptrSymbolInFile + *ptrSymbol;
		printf("st_value = %d, segAddrInFile = %d in RelocationAddr\n",\
			 sym->st_value, ptrSymbolInFile);
	}else{
		printf("an error occurs in %d in %s\n", __LINE__, __FILE__);
		exit(-1);
	}

	printf("after relocation, data = %d\n", *ptrSymbol);
}

// 重定位。
void Relocation()
{
	ELF32 elf32 = elf32LinkList->next;

	while(elf32 != NULL){
		Node segTabLinkList = elf32->segTabLinkList;
		SegNameSegTabEntry segTab= FindSegNameSegTabEntryByName(segTabLinkList, ".rel.data");
		if(segTab != NULL){
			// shdr是重定位数据的段表项。
			Elf32_Shdr *shdr = segTab->shdr;
			// Elf32_Rel *rel = shdr->sh_offset; 
			Elf32_Rel *rel = elf32->relData->addr; 
			unsigned int relNum = shdr->sh_size / 8;

			for(int i = 0; i < relNum; i++){
				Elf32_Sym *sym = elf32->symtab->addr;
				unsigned int symbolIndex = rel->r_info >> 8;
				sym += symbolIndex; 
				// void RelocationAddr(ELF32 elf32, Elf32_Rel *rel, Elf32_Sym *sym, unsigned char type)
				RelocationAddr(elf32, rel, sym, 1);

				rel++;
			}
		}

		// 必须先处理.rel.data，再处理.rel.text。
		segTab= FindSegNameSegTabEntryByName(segTabLinkList, ".rel.text");
		if(segTab != NULL){
			// shdr是重定位数据的段表项。
			Elf32_Shdr *shdr = segTab->shdr;
			// Elf32_Rel *rel = shdr->sh_offset; 
			Elf32_Rel *rel = elf32->relText->addr; 
			unsigned int relNum = shdr->sh_size / 8;

			for(int i = 0; i < relNum; i++){
				Elf32_Sym *sym = elf32->symtab->addr;
				unsigned int symbolIndex = rel->r_info >> 8;
				sym += symbolIndex; 
				// void RelocationAddr(ELF32 elf32, Elf32_Rel *rel, Elf32_Sym *sym, unsigned char type)
				RelocationAddr(elf32, rel, sym, 2);

				rel++;
			}
		}
		
		elf32 = elf32->next;
	}
}

// 组合成可执行文件。
ELF32 AssembleELF()
{
	ELF32 elf32 = (ELF32)MALLOC(sizeof(struct elf32));

	// .shstrtab。
	char *shStrTabStr = "\000.text\000.data\000.rodata\000.symtab\000.strtab\000.shstrtab\000";
	char *shStrTabStrWithoutNull = ".text.data.rodata.symtab.strtab.shstrtab";
	// shStrTabStr中的\000有7个。
	unsigned int shStrTabStrLength = strlen(shStrTabStrWithoutNull) + 7;
	Segment shstrtabSegment = (Segment)MALLOC(sizeof(struct segment));
	shstrtabSegment->addr = (void *)MALLOC(shStrTabStrLength); 
	shstrtabSegment->size = shStrTabStrLength;
	memcpy(shstrtabSegment->addr, shStrTabStr, shStrTabStrLength);

	// .strtab
	unsigned int strtabSize = 1;
	Node symNode = symDefine->next;
	while(symNode != symDefine){
		SymbolLink symlink = (SymbolLink)symNode->val.symLink;
		printf("sym name = %s\n", symlink->name);
		unsigned int size = strlen(symlink->name) + 1;
		if(size == 0){
			symNode = symNode->next;
			continue;
		}

		strtabSize += size; 
		symNode = symNode->next;
	}

	Segment strtabSegment = (Segment)MALLOC(sizeof(struct segment));
	strtabSegment->addr = (void *)MALLOC(strtabSize + 1); 
	strtabSegment->size = strtabSize + 1;

	symNode = symDefine->next;
	unsigned int strtabOffset = 1;
	while(symNode != symDefine){
		SymbolLink symlink = (SymbolLink)symNode->val.symLink;
		unsigned int size = strlen(symlink->name);
		if(size == 0){
			symNode = symNode->next;
			continue;
		}

		printf("without null sym name = %s\n", symlink->name);
		memcpy(strtabSegment->addr + strtabOffset, symlink->name, size);
		printf("without2 null sym name = %s\n", strtabSegment->addr + strtabOffset);

		strtabOffset += size + 1; 
		symNode = symNode->next;
	}

	PrintStrtabTest(strtabSegment->addr, strtabSegment->size);

	// 计算所有文件的段的长度。
	char *segNames[3] = {".text", ".data", ".rodata"};
	unsigned int segSize[3] = {0, 0, 0};

	for(int i = 0; i < 3; i++){
		unsigned int *ptrSegSize = NULL;
		char *segName = segNames[i];
		if(strcmp(segName, ".text") == 0){
			ptrSegSize = &segSize[0];
		}else if(strcmp(segName, ".data") == 0){
			ptrSegSize = &segSize[1];
		}else if(strcmp(segName, ".rodata") == 0){
			ptrSegSize = &segSize[2];
		}else{
			// TODO 不会出现这种情况。
		}

		ELF32 currentElf32 = elf32LinkList->next;
		while(currentElf32 != NULL){
			Segment segment = NULL;
			Node segTabLinkList = currentElf32->segTabLinkList;
			SegNameSegTabEntry entry = FindSegNameSegTabEntryByName(segTabLinkList, segName);
			if(entry == NULL){
				printf("error! seg %s is not exists in %d in %s\n", segName, __LINE__, __FILE__);
				currentElf32 = currentElf32->next;
				continue;
			}
	
			Elf32_Shdr *shdr = entry->shdr;
			*ptrSegSize += shdr->sh_size;
	
			currentElf32 = currentElf32->next;
		}
	}

	// 生成.text等。
	unsigned int segmentSize = sizeof(struct segment);
	Segment textSeg = (Segment)MALLOC(segmentSize);
	textSeg->addr = (void *)MALLOC(segSize[0]);
	textSeg->size = segSize[0];

	Segment dataSeg = (Segment)MALLOC(segmentSize);
	dataSeg->addr = (void *)MALLOC(segSize[1]);
	dataSeg->size = segSize[1];

	Segment rodataSeg = (Segment)MALLOC(segmentSize);
	rodataSeg->addr = (void *)MALLOC(segSize[2]);
	rodataSeg->size = segSize[2];

	void *ptrAddr = NULL;
	
	for(int i = 0; i < 3; i++){
		unsigned int *ptrSegSize = NULL;
		char *segName = segNames[i];
		if(strcmp(segName, ".text") == 0){
			ptrAddr = textSeg->addr;
		}else if(strcmp(segName, ".data") == 0){
			ptrAddr = dataSeg->addr;
		}else if(strcmp(segName, ".rodata") == 0){
			ptrAddr = rodataSeg->addr;
		}else{
			// TODO 不会出现这种情况。
		}

		ELF32 currentElf32 = elf32LinkList->next;
		while(currentElf32 != NULL){
			Segment segment = NULL;
			Node segTabLinkList = currentElf32->segTabLinkList;
			SegNameSegTabEntry entry = FindSegNameSegTabEntryByName(segTabLinkList, segName);
			if(entry == NULL){
				printf("error! seg %s is not exists in %d in %s\n", segName, __LINE__, __FILE__);
				currentElf32 = currentElf32->next;
				continue;
			}

			void *dst = (void *)((unsigned int)ptrAddr + entry->offset);
			Elf32_Shdr *shdr = entry->shdr;
			void *src = (void *)entry->addr;
			unsigned int size = entry->size;
			memcpy(dst, src, size);
	
			currentElf32 = currentElf32->next;
		}
	}

	PrintStrtabTest(strtabSegment->addr, strtabSegment->size);

	// 生成.symtab。
	// 计算.symtab的长度。
	unsigned int symSize = sizeof(Elf32_Sym);
	Node symbolLinkNode = symDefine->next;
	while(symbolLinkNode != symDefine){
		SymbolLink symlink = symbolLinkNode->val.symLink;
		Elf32_Sym *sym = symlink->sym;
		// if(sym->st_shndx == SHN_ABS || sym->st_name == 0){
		if(sym->st_name == 0){
			symbolLinkNode = symbolLinkNode->next;
			continue;
		}

		symSize += sizeof(Elf32_Sym);

		printf("2symbol name = %s\n", symbolLinkNode->val.symLink->name);
		symbolLinkNode = symbolLinkNode->next;
	}

	Segment symtabSeg = (Segment)MALLOC(segmentSize);
	symtabSeg->addr = (void *)MALLOC(symSize);
	symtabSeg->size = symSize;
	Elf32_Sym *ptrSym = (Elf32_Sym *)symtabSeg->addr;
	// 空表项。
	ptrSym->st_shndx = SHN_UNDEF;
	ptrSym++;
	symbolLinkNode = symDefine->next;
	while(symbolLinkNode != symDefine){
		SymbolLink symlink = symbolLinkNode->val.symLink;
		Elf32_Sym *sym = symlink->sym;
		// if(sym->st_shndx == SHN_ABS || sym->st_name == 0){
		if(sym->st_name == 0){
			symbolLinkNode = symbolLinkNode->next;
			continue;
		}

		PrintStrtabTest(strtabSegment->addr, strtabSize);

		printf("3symbol name = %s\n", symbolLinkNode->val.symLink->name);
		if(sym->st_shndx == SHN_ABS){
			memcpy(ptrSym, sym, sizeof(Elf32_Sym));
			// TODO 没有对GetSubStrIndex的返回值进行错误检查。
			ptrSym->st_name = GetSubStrIndex(symlink->name, strtabSegment->addr, strtabSegment->size);
			ptrSym++;
			symbolLinkNode = symbolLinkNode->next;
			continue;
		}
		memcpy(ptrSym, sym, sizeof(Elf32_Sym));
		PrintStrtabTest(strtabSegment->addr, strtabSegment->size);
		int st_name = GetSubStrIndex(symlink->name, strtabSegment->addr, strtabSegment->size);
		// TODO 暂时不检查st_name。
		// 更新st_name。因为还没有确定段表，无法更新st_shndx。
		printf("symlink->name = %s, st_name = %d\n", symlink->name, st_name);
		
		ptrSym->st_name = (Elf32_Word)st_name;
		// 更新st_shndx。
		ELF32 provider = symlink->provider;
		Segment providerShstrtab = provider->shstrtab;
		printf("st_shndx = %d\n", sym->st_shndx);
		Elf32_Shdr *shdr = (Elf32_Shdr *)provider->shdr + sym->st_shndx;
		char *segName = (char *)(providerShstrtab->addr + shdr->sh_name);
		PrintStrtabTest(providerShstrtab->addr, providerShstrtab->size);
		printf("providerShstrtab segName = %s, name = %s, index = %d\n", segName, symlink->name,\
			 sym->st_shndx);
		char *segNames[7] = {"", ".text", ".data", ".rodata", ".symtab", ".strtab", ".shstrtab"};
		int shndx = -1;
		for(int i = 0; i < 7; i++){
			if(strcmp(segName, segNames[i]) == 0){
				shndx = i;
				break;
			}
		}

		if(shndx == -1){
			printf("segName %s is not valid in %d in %s\n", segName, __LINE__, __FILE__); 
			exit(-1);
		}
		// TODO 不做错误检测。
		ptrSym->st_shndx = shndx;
		ptrSym++;
		symbolLinkNode = symbolLinkNode->next;
	}

	// 段表。
//	char *shStrTabStr = "\000.text\000.data\000.rodata\000.symtab\000.strtab\000.shstrtab\000";
//	char *shStrTabStrWithoutNull = ".text.data.rodata.symtab.strtab.shstrtab";
	unsigned int shdrSize = sizeof(Elf32_Shdr);
	Elf32_Shdr *shdrs[7] = {NULL};
	Elf32_Shdr *shdr = (Elf32_Shdr *)MALLOC(shdrSize * 7);
	Elf32_Shdr *ptrShdr = shdr;
	// 空表项。
	ptrShdr++;

	
//	char *shStrTabStrWithoutNull = ".text.data.rodata.symtab.strtab.shstrtab";
//	unsigned int offset = segSize[0] + segSize[1] + segSize[2];
//	offset += 52;
//	offset += sizeof(Elf32_Phdr) * 3;
	unsigned int offset = segOffsetLists[2] + segSize[2];
	char *ptrShStrTabStr = shStrTabStr;
	ptrShStrTabStr++;
	for(int i = 1; i < 7; i++){
		printf("ptrShStrTabStr = %s\n", ptrShStrTabStr);
		ptrShdr->sh_name = GetSubStrIndex(ptrShStrTabStr, shStrTabStr, shStrTabStrLength);
		
//	char *segNames[3] = {".text", ".data", ".rodata"};
//	unsigned int segSize[3] = {0, 0, 0};
		if(strcmp(ptrShStrTabStr, ".bss") == 0){
		    ptrShdr->sh_type = (Elf32_Word)SHT_NOBITS;
		    ptrShdr->sh_flags = (Elf32_Word)SHF_ALLOC + SHF_WRITE;

			ptrShdr->sh_addralign = (Elf32_Word)4;
		}else if(strcmp(ptrShStrTabStr, ".data") == 0){
		    ptrShdr->sh_type = (Elf32_Word)SHT_PROGBITS;
		    ptrShdr->sh_flags = (Elf32_Word)SHF_ALLOC + SHF_WRITE;
			ptrShdr->sh_addr = segLists[1]->next->val.segTab->shdr->sh_addr;
			ptrShdr->sh_offset = segLists[1]->next->val.segTab->shdr->sh_offset;

			ptrShdr->sh_size = segSize[1];
			// gcc设置sh_addralign的值是4。
			ptrShdr->sh_addralign = (Elf32_Word)4;
		}else if(strcmp(ptrShStrTabStr, ".rodata") == 0){
		    ptrShdr->sh_type = (Elf32_Word)SHT_PROGBITS;
		    ptrShdr->sh_flags = (Elf32_Word)SHF_ALLOC;
			ptrShdr->sh_addr = segLists[2]->next->val.segTab->shdr->sh_addr;
			ptrShdr->sh_offset = segLists[2]->next->val.segTab->shdr->sh_offset;

			ptrShdr->sh_size = segSize[2];
			ptrShdr->sh_addralign = (Elf32_Word)1;
		}else if(strcmp(ptrShStrTabStr, ".shstrtab") == 0){
		    ptrShdr->sh_type = (Elf32_Word)SHT_STRTAB;
		    ptrShdr->sh_flags = (Elf32_Word)0;      // none

		    ptrShdr->sh_size = shstrtabSegment->size;
			ptrShdr->sh_addralign = (Elf32_Word)1;

			ptrShdr->sh_offset = offset;
			offset += ptrShdr->sh_size;
		}else if(strcmp(ptrShStrTabStr, ".strtab") == 0){
		    ptrShdr->sh_type = (Elf32_Word)SHT_STRTAB;
		    // todo 不知道怎么处理。在《程序员的自我修养》3.4节有资料。
		    ptrShdr->sh_flags = (Elf32_Word)0;

			ptrShdr->sh_size = strtabSegment->size;
			ptrShdr->sh_addralign = (Elf32_Word)1;

			ptrShdr->sh_offset = offset;
			offset += ptrShdr->sh_size;
		}else if(strcmp(ptrShStrTabStr, ".symtab") == 0){
		    ptrShdr->sh_type = (Elf32_Word)SHT_SYMTAB;
		    // todo 不知道怎么处理。在《程序员的自我修养》3.4节有资料。
		    ptrShdr->sh_flags = (Elf32_Word)0;

			ptrShdr->sh_size = symtabSeg->size;
			ptrShdr->sh_addralign = (Elf32_Word)4;
			ptrShdr->sh_entsize=(Elf32_Word)sizeof(Elf32_Sym);  

			// TODO 暂时不知道怎么设置。
			ptrShdr->sh_link = (Elf32_Word)5;
			// 花了很多很多时间才弄明白这个值是什么。它是LOCAL变量的数目。
			// sh_info = (Elf32_Word)SYM_TAB;
			// TODO 待补充。
			ptrShdr->sh_info = 12;

			ptrShdr->sh_offset = offset;
			offset += ptrShdr->sh_size;
			
		}else if(strcmp(ptrShStrTabStr, ".text") == 0){
		    ptrShdr->sh_type = (Elf32_Word)SHT_PROGBITS;
		    ptrShdr->sh_flags = (Elf32_Word)SHF_ALLOC + SHF_EXECINSTR;

			ptrShdr->sh_addr = segLists[0]->next->val.segTab->shdr->sh_addr;
			ptrShdr->sh_offset = segLists[0]->next->val.segTab->shdr->sh_offset;

			ptrShdr->sh_size = segSize[0];
			ptrShdr->sh_addralign = (Elf32_Word)1;
		}else{
			// TODO 不会出现这种情况。
		}

		ptrShdr->sh_name = (Elf32_Word)GetSubStrIndex(ptrShStrTabStr, shStrTabStr, shStrTabStrLength); 
		printf("ptrShdr->sh_name = %d, sh_name = %s\n", ptrShdr->sh_name, \
			shStrTabStr + ptrShdr->sh_name);

		ptrShdr++;
		ptrShStrTabStr += strlen(ptrShStrTabStr) + 1;
	}

	// 程序头表。
	unsigned int phdrSize = sizeof(Elf32_Phdr);
	// Elf32_Phdr *phdr = (Elf32_Phdr *)MALLOC(phdrSize * 4);
	// TODO 是否应该设置一个空表项？暂时不设置。
	Elf32_Phdr *phdr = (Elf32_Phdr *)MALLOC(phdrSize * 3);

	Elf32_Phdr *ptrPhdr = phdr;

//	Elf32_Word    p_type;         /* Segment type */
//	Elf32_Off p_offset;       /* Segment file offset */
//	Elf32_Addr    p_vaddr;        /* Segment virtual address */
//	Elf32_Addr    p_paddr;        /* Segment physical address */
//	Elf32_Word    p_filesz;       /* Segment size in file */
//	Elf32_Word    p_memsz;        /* Segment size in memory */
//	Elf32_Word    p_flags;        /* Segment flags */
//	Elf32_Word    p_align;        /* Segment alignment */	

	// p_flags数组。
	// char *segNames[3] = {".text", ".data", ".rodata"};
	unsigned int flags[3] = {PF_R | PF_X, PF_R | PF_W, PF_R};

	for(int i = 0; i < 3; i++){
		Elf32_Shdr *shdr = segLists[i]->next->val.segTab->shdr;

		ptrPhdr->p_type = PT_LOAD; 
		ptrPhdr->p_offset = shdr->sh_offset;
		ptrPhdr->p_vaddr = shdr->sh_addr;
		ptrPhdr->p_paddr = shdr->sh_addr;
		unsigned int size = segSize[i];
	//	ptrPhdr->p_filesz = shdr->sh_size;
	//	ptrPhdr->p_memsz = shdr->sh_size;
		ptrPhdr->p_filesz = size;
		ptrPhdr->p_memsz = size;
		ptrPhdr->p_flags = flags[i];
		// TODO 不知道怎么处理。
		ptrPhdr->p_align = 0x1000;

		ptrPhdr++;
	}

	// ELF文件头。
	Elf32_Ehdr *ehdr = GenerateELFHeader();
	char *entryFunc = "nomain";

	symNode = symDefine->next;
	while(symNode != symDefine){
		SymbolLink symlink = (SymbolLink)symNode->val.symLink;
		unsigned int size = strlen(symlink->name);
		if(size == 0){
			symNode = symNode->next;
			continue;
		}

		if(strcmp(entryFunc, symlink->name) == 0){
			// TODO e_entry的默认值是0。当它是默认值时，判定为错误。
			ehdr->e_entry = symlink->sym->st_value;
			printf("e_entry = %x\n", ehdr->e_entry);
			printf("e_entry = %d\n", 3);
			break;
		}

		symNode = symNode->next;
	}

	if(ehdr->e_entry == 0){
		printf("Entry point address should not be 0\n");
		exit(-1);
	}

	// e_shoff。过了好一会儿才重新想起e_shoff是什么。
	unsigned int eShoff = 0;
	eShoff += segOffsetLists[2] + segSize[2];
//	eShoff += 52;	// ELF文件头。
//	eShoff += phdrSize * 3;	// 程序头。
//	// segSize
//	for(int i = 0; i < 3; i++){
//		eShoff += segSize[i];	// 代码、数据等。
//	}
	// .text.data.rodata.symtab.strtab.shstrtab
	eShoff += symtabSeg->size;
	eShoff += strtabSegment->size;
	eShoff += shstrtabSegment->size;

	ehdr->e_shoff = eShoff;

	elf32->ehdr = ehdr;
	elf32->phdr = phdr;
	elf32->shdr = shdr;
	elf32->text = textSeg;
	elf32->data = dataSeg;
	elf32->rodata = rodataSeg;
	elf32->symtab = symtabSeg;
	elf32->strtab = strtabSegment;
	elf32->shstrtab = shstrtabSegment;

	return elf32;
}

void WriteElf(ELF32 elf32)
{
	char *filename = "ab";

	FILE *file = fopen(filename, "wb");

	// 写入文件头。
	fwrite(elf32->ehdr, 52, 1, file);
	// 写入程序头。
	fwrite(elf32->phdr, sizeof(Elf32_Phdr) * 3, 1, file);
	// 依次写入.text.data.rodata.symtab.strtab.shstrtab
	Segment textSeg = elf32->text;
	fseek(file ,segOffsetLists[0], SEEK_SET);
	fwrite(textSeg->addr, textSeg->size, 1, file);

	Segment dataSeg = elf32->data;
	fseek(file ,segOffsetLists[1], SEEK_SET);
	fwrite(dataSeg->addr, dataSeg->size, 1, file);

	Segment rodataSeg = elf32->rodata;
	fseek(file ,segOffsetLists[2], SEEK_SET);
	fwrite(rodataSeg->addr, rodataSeg->size, 1, file);

	Segment symtabSeg = elf32->symtab;
	fwrite(symtabSeg->addr, symtabSeg->size, 1, file);

	Segment strtabSeg = elf32->strtab;
	fwrite(strtabSeg->addr, strtabSeg->size, 1, file);

	Segment shstrtabSeg = elf32->shstrtab;
	fwrite(shstrtabSeg->addr, shstrtabSeg->size, 1, file);

	// 写入段表。
	fwrite(elf32->shdr, sizeof(Elf32_Shdr) * 7, 1, file);

	int result = fclose(file);

	if(result == EOF){
		printf("create file %s failure\n", filename);
	}	
}

void MergeRodata(Segment src, Segment dst)
{
	if(src == NULL)	return;

	void *srcAddr = src->addr;
	unsigned int len = src->size;

	void *dstAddr = dst->addr;
	dst->addr += len;
	dst->size += len;

	memcpy(dstAddr, srcAddr, len);
}

void MergeRelData(Segment src, Segment dst)
{
	if(src == NULL)	return;

	void *srcAddr = src->addr;
	unsigned int len = src->size;

	void *dstAddr = dst->addr;
	dst->addr += len;
	dst->size += len;

	memcpy(dstAddr, srcAddr, len);
}

void MergeData(Segment src, Segment dst, Segment relData, unsigned int rodataSize)
{
	if(src == NULL)	return;

	void *srcAddr = src->addr;
	unsigned int delta = src->size;
	// 先通过rel.data找到要处理的.data。
	Elf32_Rel *rel = NULL;
	unsigned int relSize = sizeof(Elf32_Rel);
	unsigned int relNum = 0;
	if(relData == NULL){
		relNum = 0;
		rel = NULL;
	}else{
		relNum = relData->size / relSize;
		rel = (Elf32_Rel *)(relData->addr);
	}
	for(int i = 0; i < relNum; i++){
		Elf32_Addr	offset = rel->r_offset;
		// TODO 搁置重定位类型。
		// 暂且认为重定位类型都是绝对重定位。
		unsigned int val = *((unsigned int *)(srcAddr + offset));
		// 此处增量应该是.rodata的增量。
		// 必须先合并.rodata，再合并.data，因为.data的新数据依赖.rodata的长度。
		// 好像不正确。究竟应该是怎样的呢？增量是合并.rodata前旧.rodata的长度。
		unsigned int newVal = val + rodataSize;
		*((unsigned int *)(srcAddr + offset)) = newVal;
		// 更新.rel.data。其实可以不必更新。因为.rel.data似乎无用了。
		rel->r_offset = newVal;

		rel++;
	}

	void *dstAddr = dst->addr;
	dst->addr += delta;
	dst->size += delta;

	// 合并.data。
	memcpy(dstAddr, srcAddr, delta);

	// 合并.rel.data。
	// 其实不必合并。因为在可执行文件中没有.rel.data。
}

void MergeRelText(Segment relTextSrc, Segment relTextDst, Segment textDst, Segment symtab, char *strtab)
{
	if(relTextSrc == NULL)	return;

	Elf32_Rel *rel = (Elf32_Rel *)(relTextSrc->addr);
	unsigned int relSize = sizeof(Elf32_Rel);
	unsigned int relTextSrcLen = relTextSrc->size;
	unsigned int relNum = relTextSrcLen / relSize;

	unsigned int deltaText = textDst->size;

	unsigned int symtabNum = symtab->size / sizeof(Elf32_Sym);
	for(int i = 0; i < relNum; i++){
		// 更新.rel.text。
		// 怎么计算新symbol index？不需要遍历.symtab，只需让旧symbol index加上旧.symtab的长度即可。
		// 不对，不是加上“旧.symtab的长度”，而是加上symtabNum。
		// 怎么计算offset？加上旧.text的长度。在这里，是deltaText。
		Elf32_Addr offset = rel->r_offset;
		Elf32_Word info = rel->r_info;
		unsigned int symbolIndex = info >> 8;
		// strtab是这样的num1\000num2。
		char *name = (char *)(strtab + symbolIndex);
		// 再找到name在新.strtab中的偏移量就可以了。
		// 新.strtab是多少？比较麻烦，需要好好想一想。

		rel->r_offset = offset + deltaText;
		rel->r_info = ((symbolIndex + symtabNum) << 8 ) | (info & 0xFF);

		rel++;
	}

	memcpy(relTextDst->addr, relTextSrc->addr, relTextSrc->size);
	relTextDst->addr += relTextSrc->size;
	relTextDst->size += relTextSrc->size;
}

void MergeSymtab()
{
	Node localSymLinkList = InitLinkList();
	Node globalSymLinkList = InitLinkList();
	ELF32 elf32 = elf32LinkList->next;
	int i = 0;
	unsigned int rodataSize = 0;
	unsigned int dataSize = 0;
	unsigned int textSize = 0;
	while(elf32 != NULL){
		printf("elf32--%d\n", i++);
		Elf32_Shdr *shdr = elf32->shdr;
		char *shstrtab = elf32->shstrtab->addr;
		// 处理.symtab：
		// 1. 把.symtab分成local和global两类。
		// 2. 查询符合的name。
		Elf32_Sym *sym = (Elf32_Sym *)elf32->symtab->addr;

//		textSize += elf32->text->size;
//		dataSize += elf32->data->size;
//		rodataSize += elf32->rodata->size;

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
			
			Elf32_Section shndx = sym->st_shndx;
			if(shndx == SHN_ABS || shndx == SHN_COMMON || shndx == SHN_UNDEF){
				printf("segName = %s\n", "special value");
			}else{
				Elf32_Shdr *currentShdr = shdr + shndx;
				Elf32_Word name = currentShdr->sh_name;
				char *segName = shstrtab + name;
				printf("segName = %s\n", segName);
				if(strcmp(segName, ".text") == 0){
					sym->st_value += textSize; 
					// textSize += elf32->text->size;
				}else if(strcmp(segName, ".data") == 0){
					sym->st_value += dataSize; 
					// dataSize += elf32->data->size;
				}else if(strcmp(segName, ".rodata") == 0){
					sym->st_value += rodataSize; 
					// rodataSize += elf32->rodata->size;
				}else{
					// TODO 怎么做？
				}
			}

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

			if(elf32->text != NULL){
				textSize += elf32->text->size;
			}

			if(elf32->data != NULL){
				dataSize += elf32->data->size;
			}

			if(elf32->rodata != NULL){
				rodataSize += elf32->rodata->size;
			}
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

	// 更新符号的st_name。
	currentSymLinkNode = trimSymLinkList->next;
	while(currentSymLinkNode != trimSymLinkList){
		char *name = currentSymLinkNode->val.symLink->name;
		// int GetSubStrIndex(char *subStr, char *str, unsigned int strLength)
		int index = GetSubStrIndex(name, trimStrtabStart, trimStrtabSize);
		// TODO 对此处的异常，我并不知道怎么处理。像这样处理，只是为了发现错误。
		// 正常情况下，strtab和symtab的名称必须是一一对应的。
		// 如果二者不是一一对应的，那么，前面的产生的数据有问题。
		if(index == -1){
			printf("%s does not exist in %d in %s", name, __LINE__, __FILE__);
			exit(-1);
		}
		currentSymLinkNode->val.symLink->sym->st_name = (Elf32_Word)index;
		currentSymLinkNode = currentSymLinkNode->next;
	}
}

void MergeText(Segment textSrc, Segment textDst)
{
	if(textSrc == NULL)	return;

	memcpy(textDst->addr, textSrc->addr, textSrc->size);	
	textDst->addr += textSrc->size;
	textDst->size += textSrc->size;
}

void MergeSegment()
{
	ELF32 elf32 = elf32LinkList->next;

	unsigned int size = sizeof(struct segment);

	Segment text = (Segment)MALLOC(size);
	text->addr = (void *)MALLOC(textSize);

	Segment data = (Segment)MALLOC(size);
	data->addr = (void *)MALLOC(dataSize);

	Segment rodata = (Segment)MALLOC(size);
	rodata->addr = (void *)MALLOC(rodataSize);

	Segment relText = (Segment)MALLOC(size);
	relText->addr = (void *)MALLOC(relTextSize);

	Segment relData = (Segment)MALLOC(size);
	relData->addr = (void *)MALLOC(relDataSize); 

	Segment symtab = (Segment)MALLOC(size);
	symtab->addr = (void *)MALLOC(symtabSize); 

	while(elf32 != NULL){
		// 合并.data。
		Segment elf32Rodata = elf32->rodata;
		unsigned int rodataSize = elf32Rodata != NULL ? elf32Rodata->size : 0;
		// MergeData(Segment *src, Segment *dst, Segment *relData, unsigned int rodataSize)
		MergeData(elf32->data, data, elf32->relData, rodataSize);
		// 合并.rodata。
		MergeRodata(elf32->rodata, rodata);
		// 合并.rel.text。
		// MergeRelText(Segment *relTextSrc, Segment *relTextDst, Segment *textDst, Segment *symtab)
		MergeRelText(elf32->relText, relText, text, symtab, "");
		// 合并.symtab。
		// MergeSymtab(elf32->symtab, symtab);
		// 合并.text。
		MergeText(elf32->text, text);	

		elf32 = elf32->next;
	}
	// 合并.symtab。
	MergeSymtab();
}
