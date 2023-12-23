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

void MergeRelText(Segment relTextSrc, Segment relTextDst, Segment textDst, Segment symtab)
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
		MergeRelText(elf32->relText, relText, text, symtab);
		// 合并.symtab。
		MergeSymtab(elf32->symtab, symtab);
		// 合并.text。
		MergeText(elf32->text, text);	

		elf32 = elf32->next;
	}
}
