#ifndef LINKER_H
#define LINKER_H

#include "common.h"

typedef struct elf32{
	// ELF文件头。
	Elf32_Ehdr *ehdr;
	// 程序头。
	Elf32_Phdr *phdr;	
	// 段表。
	Elf32_Shdr *shdr;
	// .text
	unsigned int *text;
	// .symtab
	Elf32_Sym *symtab;
	// .rel.data
	Elf32_Rel *relData;
	// .rel.text
	Elf32_Rel *relText;
	// .rodata
	unsigned int *rodata;
	// .data
	unsigned int *data;
	// .strtab
	unsigned char *strtab;
	// .shstrtab
	unsigned char *shstrtab;

//	struct elf32 *pre;
	struct elf32 *next;
} *ELF32;

// static ELF32 elf32LinkList = NULL;
// static ELF32 elf32LinkList;
ELF32 elf32LinkList;
ELF32 preElf32;

void AppendElf32LinkList(ELF32 node);

#endif
