#ifndef LINKER_H
#define LINKER_H

#include "common.h"
#include "linklist.h"

typedef struct segment{
//	unsigned int addr;
	void *addr;
	unsigned int size;
	unsigned int shndx;
} *Segment;

typedef struct elf32{
	// ELF文件头。
	Elf32_Ehdr *ehdr;
	// 程序头。
	Elf32_Phdr *phdr;	
	// 段表。
	Elf32_Shdr *shdr;
	// .text
//	unsigned int *text;
	// unsigned char *text;
	Segment text;
	// .symtab
//	Elf32_Sym *symtab;
	Segment symtab;
	// .rel.data
//	Elf32_Rel *relData;
	Segment relData;
	// .rel.text
//	Elf32_Rel *relText;
	Segment relText;
	// .rodata
//	unsigned int *rodata;
//	unsigned char *rodata;
	Segment rodata;
	// .data
//	unsigned int *data;
//	unsigned char *data;
	Segment data;
	// .strtab
//	unsigned char *strtab;
	Segment strtab;
	// .shstrtab
//	unsigned char *shstrtab;
	Segment shstrtab;

//	struct elf32 *pre;
	struct elf32 *next;
} *ELF32;

// static ELF32 elf32LinkList = NULL;
// static ELF32 elf32LinkList;
ELF32 elf32LinkList;
ELF32 preElf32;
//unsigned int textSize = 0;
//unsigned int dataSize = 0;
//unsigned int rodataSize = 0;
//unsigned int relDataSize = 0;
//unsigned int relTextSize = 0;
//unsigned int symtabSize = 0;
//unsigned int strtabSize = 0;

unsigned int textSize ;// 0;
unsigned int dataSize ;// 0;
unsigned int rodataSize ;// 0;
unsigned int relDataSize ;// 0;
unsigned int relTextSize ;// 0;
unsigned int symtabSize ;// 0;
unsigned int strtabSize ;// 0;

Node ExplodeStrtab(char *strtab, unsigned int size);
Segment UniqueLinkListStr(Node list);
void AppendElf32LinkList(ELF32 node);
void CollectInfo(unsigned int num, char **filenames);
void MergeRodata(Segment src, Segment dst);
void MergeRelData(Segment src, Segment dst);
void MergeData(Segment src, Segment dst, Segment relData, unsigned int rodataSize);
void MergeRelText(Segment relTextSrc, Segment relTextDst, Segment textDst, Segment symtab, char *strtab);
void MergeSymtab();
void MergeText(Segment textDst, Segment textSrc);
void MergeSegment();

#endif
