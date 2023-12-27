#ifndef LINK_LIST_H
#define LINK_LIST_H

#include "common.h"

typedef struct segment{
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

// 段名--段表项。
typedef struct segNameSegTabEntry{
	char *segName;
	Elf32_Shdr *shdr;
} *SegNameSegTabEntry;

typedef struct symbolLink{
	char *name;
	Elf32_Sym *sym;
	ELF32 provider;
	ELF32 receiver;
} *SymbolLink;

typedef enum{
	NUM, STRING, SYMBOL_LINK, NODE_VALUE_SEG_TAB
} NODE_VALUE;

typedef union value{
	int num;
	char *str;
	SymbolLink symLink;
	SegNameSegTabEntry segTab;
} Value;

typedef struct node{
	Value val;
	NODE_VALUE type;	
	struct node *pre;
	struct node *next;
} *Node;

Node InitLinkList();

void AppendNode(Node list, Node node);
// TODO 链表的节点的值都不相同，查找才有意义。
// 当值是字符串、整型时，根据值可以查找节点。当节点的值是其他，例如结构体，我不知道怎么查找。
// 扩展？以后会遇到什么情况还是未知，我没法现在设计出能处理未知情况的代码。
// 先写完再说。
Node FindNodeByStr(Node list, char *str);
Node FindNodeByNum(Node list, int num);
Node MergeLinkList(Node list1, Node list2);

#endif
