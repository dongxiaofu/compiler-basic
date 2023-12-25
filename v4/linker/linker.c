#include "linker.h"

Node ExplodeStrtab(char *strtab, unsigned int size)
{
	Node head = InitLinkList();
	unsigned int len = 0;
	while(len < size){
		char *str = strtab + len;
		unsigned int strLen = strlen(strtab) + 1;
		
		Node node = (Node)MALLOC(sizeof(struct node)); 
		node->type = STRING;
		node->val.str = (char *)MALLOC(strLen); 
		memcpy(node->val.str, str, strLen);
		
		AppendNode(head, node);

		len += strLen;
	}

	return head;
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

void CollectInfo(unsigned int num, char **filenames)
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
}

void MergeRodata(Segment src, Segment dst)
{
	void *srcAddr = src->addr;
	unsigned int len = src->size;

	void *dstAddr = dst->addr;
	dst->addr += len;
	dst->size += len;

	memcpy(dstAddr, srcAddr, len);
}

void MergeRelData(Segment src, Segment dst)
{
	void *srcAddr = src->addr;
	unsigned int len = src->size;

	void *dstAddr = dst->addr;
	dst->addr += len;
	dst->size += len;

	memcpy(dstAddr, srcAddr, len);
}

void MergeData(Segment src, Segment dst, Segment relData, unsigned int rodataSize)
{
	void *srcAddr = src->addr;
	unsigned int delta = src->size;
	// 先通过rel.data找到要处理的.data。
	Elf32_Rel *rel = (Elf32_Rel *)(relData->addr);
	unsigned int relSize = sizeof(Elf32_Rel);
	unsigned int relNum = relData->size / relSize;
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

void MergeSymtab(Segment src, Segment dst)
{
	memcpy(dst->addr, src->addr, src->size);	
	dst->addr += src->size;
	dst->size += src->size;
}

void MergeText(Segment textSrc, Segment textDst)
{
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
		// MergeData(Segment *src, Segment *dst, Segment *relData, unsigned int rodataSize)
		MergeData(elf32->data, data, elf32->relData, elf32->rodata->size);
		// 合并.rodata。
		MergeRodata(elf32->rodata, rodata);
		// 合并.rel.text。
		// MergeRelText(Segment *relTextSrc, Segment *relTextDst, Segment *textDst, Segment *symtab)
		MergeRelText(elf32->relText, relText, text, symtab, "");
		// 合并.symtab。
		MergeSymtab(elf32->symtab, symtab);
		// 合并.text。
		MergeText(elf32->text, text);	

		elf32 = elf32->next;
	}
}
