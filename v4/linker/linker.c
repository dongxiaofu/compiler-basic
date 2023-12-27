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

		unsigned int size = 0;
		Elf32_Sym *sym = (Elf32_Sym *)symSeg->addr;
		while(size < symSeg->size){
			SymbolLink symbolLink = (SymbolLink)MALLOC(sizeof(struct symbolLink));
			symbolLink->name = strtab + sym->st_name;
			symbolLink->sym = sym;
			
			unsigned char bind = ELF32_ST_BIND(sym->st_info);
			if(bind == STB_GLOBAL){
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


		elf32->ehdr = ehdr;
		elf32->phdr = phdr;
		elf32->shdr = shdr;

		AppendElf32LinkList(elf32);
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
