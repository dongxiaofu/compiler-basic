#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "input.h"
#include "decl.h"
#include "lex.h"
#include "ast.h"
#include "packages.h"
#include "symbol.h"

#include <signal.h>
#include <unistd.h>
#include <execinfo.h>


#define BACKTRACE_SIZE 1024

Heap CurrentHeap;
struct heap ProgramHeap;
HEAP(ProgramHeap);

int main(int argc, char *argv[])
{
	printf("I am a scanner\n");

	if(argc < 2){
		ERROR("need filename", "");
		exit(-1);
	}	

	CurrentHeap = &ProgramHeap;
	ReadSourceFile(argv[1]);	
	// 初始化扫描器
	setupScanner();

	Token token;
	int i = 0;

	cursor_tail = (START_CURSOR_LINK)malloc(sizeof(struct start_cursor_link));
	char_tail = (START_CHAR_LINK)malloc(sizeof(struct start_char_link));		

	cursor_tail->pre = cursor_tail->next = cursor_tail->start_cursor = NULL;
	char_tail->pre = char_tail->next = char_tail->start_char = NULL;

	current_token_tail = (TOKEN_LINK)malloc(sizeof(struct token_link));
	current_token_tail->pre = current_token_tail->next = current_token_tail->token = NULL;
	Token *tokenPtr = (Token *)malloc(sizeof(Token));
	if(tokenPtr == NULL){
		perror("init tokenPtr error\n");
		exit(-1);
	}
	current_token_tail->token = tokenPtr;

		get_token();
		if(current_token.kind == TK_EOF) return 0;
		// 语法分析
		AstTranslationUnit transUnit = ParseTranslationUnit();

		FUNCTION_LIST = NULL;
		INTERFACE_LIST = NULL;

		SetupTypeSystem();
		InitSymbolTable();

		// 语义分析
		CheckTranslationUnit(transUnit);
		// 翻译成中间代码
		Translate(transUnit);
	
	return 0;
}

AstTranslationUnit ParseTranslationUnit(){
	
	AstTranslationUnit p;
//	CREATE_AST_NODE(p, FunctionTest);
	CREATE_AST_NODE(p, TranslationUnit);
	int i = 0;
	while(0){
		NO_TOKEN;	
		ParseAssignmentsStmt();
		printf("【%d assign over】\n", i++);
	}
	
	p = ParseSourceFile();
	LOG("parse unit over\n");

	return p;
}

void *MALLOC(int size)
{
//	void *p = (void *)malloc(size);
//	if(p == NULL){
//		exit(-4);
//	}

//	memset(p, 0, size);
	void *p = HeapAllocate(CurrentHeap, size);
	memset(p, 0, size);
	return p;
}

int HeapAllocate(Heap heap, int size)
{
	struct mblock *blk = heap->last;

	// size = ALIGN(size, sizeof(union align));

	while(size > blk->end - blk->avail){
		int m = 4096 + sizeof(struct mblock) + size; 
		blk->next = (struct mblock *)malloc(m);
		blk = blk->next;
		if(blk == NULL){
			printf("分配内存失败\n");
			exit(-4);
		}
		blk->begin = blk->avail = (char *)(blk + 1);
//		blk->begin = blk->avail = (char *)(blk);
		blk->next = NULL;
		heap->last = blk;	
		// blk->end = (char *)(blk + m);
		blk->end = (char *)blk + m;
	}

	blk->avail += size;

	return blk->avail - size;
}
