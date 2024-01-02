#ifndef LINKER_H
#define LINKER_H

#include "common.h"
#include "linklist.h"

//typedef struct segment{
//	void *addr;
//	unsigned int size;
//	unsigned int shndx;
//} *Segment;
//
//typedef struct elf32{
//	// ELF文件头。
//	Elf32_Ehdr *ehdr;
//	// 程序头。
//	Elf32_Phdr *phdr;	
//	// 段表。
//	Elf32_Shdr *shdr;
//	// .text
////	unsigned int *text;
//	// unsigned char *text;
//	Segment text;
//	// .symtab
////	Elf32_Sym *symtab;
//	Segment symtab;
//	// .rel.data
////	Elf32_Rel *relData;
//	Segment relData;
//	// .rel.text
////	Elf32_Rel *relText;
//	Segment relText;
//	// .rodata
////	unsigned int *rodata;
////	unsigned char *rodata;
//	Segment rodata;
//	// .data
////	unsigned int *data;
////	unsigned char *data;
//	Segment data;
//	// .strtab
////	unsigned char *strtab;
//	Segment strtab;
//	// .shstrtab
////	unsigned char *shstrtab;
//	Segment shstrtab;
//
////	struct elf32 *pre;
//	struct elf32 *next;
//} *ELF32;

//typedef struct symbolLink{
//	char *name;
//	Elf32_Sym *sym;
//	ELF32 provider;
//	ELF32 receiver;
//} *SymbolLink;

// static ELF32 elf32LinkList = NULL;
// static ELF32 elf32LinkList;
ELF32 elf32LinkList;
ELF32 preElf32;
Node symDefine;
Node symLink;
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

SegNameSegTabEntry FindSegNameSegTabEntryByIndex(Node segTabLinkList, unsigned int index);
SegNameSegTabEntry FindSegNameSegTabEntryByName(Node segTabLinkList, char *segName);
void AllocSegmentAddress(char *segName, unsigned int *base);
void AllocAddress(unsigned int *base);
void CollectInfo();
void ReadElf(unsigned int num, char **filenames);
SymbolLink FindSymbolLinkBySymName(Node list, char *symName);
SymbolLink FindSymbolLinkBySymIndex(Node list, unsigned int index);
void ParseSym();
void RelocationAddr(ELF32 elf32, Elf32_Rel *rel, Elf32_Sym *sym, unsigned char type);
void Relocation();
ELF32 AssembleELF();

void MergeRodata(Segment src, Segment dst);
void MergeRelData(Segment src, Segment dst);
void MergeData(Segment src, Segment dst, Segment relData, unsigned int rodataSize);
void MergeRelText(Segment relTextSrc, Segment relTextDst, Segment textDst, Segment symtab, char *strtab);
void MergeSymtab();
void MergeText(Segment textDst, Segment textSrc);
void MergeSegment();

#endif
