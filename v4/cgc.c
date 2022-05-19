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

void seg()
{
	int *p = NULL;
	*p = 4;
}

void segv_handler(int sig)
{
	void *func[BACKTRACE_SIZE];
	int j=0, nptrs=0;
	 char **symb = NULL;
	
	//char **symb = NULL;
	
	int size;
	
	printf("<----------BACKTRACE START---------->\n");
	printf("signo: %d\n", sig);

	printf("1\n");
	size = backtrace(func, BACKTRACE_SIZE);
	printf("2\n");
	printf("backtrace() returned %d addresses\n", size);
	
//	backtrace_symbols_fd(func, size, STDERR_FILENO);

//方法2
	symb = backtrace_symbols(func, size);
	if (symb == NULL) {
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}
	
	printf("3\n");
	printf("nptrs = %d\n", nptrs);
	for (j = 0; j < size; j++)
		printf("%s\n", symb[j]);
	
	free(symb);
	symb = NULL;
	
	printf("<----------BACKTRACE END---------->\n");
	
	exit(1);
}

Heap CurrentHeap;
struct heap ProgramHeap;
HEAP(ProgramHeap);

int main(int argc, char *argv[])
{

//	CURRENT_HEAP = {&CURRENT_HEAP.head};

//		signal(SIGSEGV, segv_handler);
//		signal(SIGINT, segv_handler);
//		signal(SIGQUIT, segv_handler);
	printf("I am a scanner\n");

//	printf("argc = %d\n", argc);
//	for(int i = 0; i < argc; i++){
//		printf("argv[%d] = %s\n", i, argv[i]);
//	}

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

	// START_CURSOR_LINK cursor_tail = (START_CURSOR_LINK)malloc(sizeof(*START_CURSOR_LINK));
	// START_CHAR_LINK char_tail = (START_CHAR_LINK)malloc(sizeof(*START_CHAR_LINK));		
	// cursor_tail = (START_CURSOR_LINK)malloc(sizeof(*START_CURSOR_LINK));
	// char_tail = (START_CHAR_LINK)malloc(sizeof(*START_CHAR_LINK));		
	// cursor_tail = (START_CURSOR_LINK)malloc(sizeof(*cursor_tail));
	// char_tail = (START_CHAR_LINK)malloc(sizeof(*char_tail));		
	cursor_tail = (START_CURSOR_LINK)malloc(sizeof(struct start_cursor_link));
	char_tail = (START_CHAR_LINK)malloc(sizeof(struct start_char_link));		

	cursor_tail->pre = cursor_tail->next = cursor_tail->start_cursor = NULL;
	char_tail->pre = char_tail->next = char_tail->start_char = NULL;

	// current_token_tail = (TOKEN_LINK)malloc(sizeof(*TOKEN_LINK));
	current_token_tail = (TOKEN_LINK)malloc(sizeof(struct token_link));
//	Token *tokenPtr = (Token *)malloc(sizeof(Token));
//	if(tokenPtr == NULL){
//		perror("init tokenPtr error\n");
//		exit(-1);
//	}
//	current_token_tail->token = tokenPtr;
//	current_token_tail->token = (Token *)malloc(sizeof(Token));
	// error: incompatible types when assigning to type 'Token' {aka 'struct token'} from type 'void *'
	current_token_tail->pre = current_token_tail->next = current_token_tail->token = NULL;
	Token *tokenPtr = (Token *)malloc(sizeof(Token));
	if(tokenPtr == NULL){
		perror("init tokenPtr error\n");
		exit(-1);
	}
	current_token_tail->token = tokenPtr;


	int loop_counter = 0;
	while(0){
		if(++loop_counter > 120){
			break;
		}
		NO_TOKEN;
		get_token();
	}
//	printf("scan token over\n");
//	exit(3);
	
	//	signal(SIGSEGV, segv_handler);
	//	signal(SIGINT, segv_handler);
	//	signal(SIGQUIT, segv_handler);
		get_token();
		if(current_token.kind == TK_EOF) return 0;
		// 语法分析
		AstTranslationUnit transUnit = ParseTranslationUnit();

		//signal(SIGSEGV, segv_handler);
		FUNCTION_LIST = NULL;
		INTERFACE_LIST = NULL;

		SetupTypeSystem();
		InitSymbolTable();

		// 语义分析
		CheckTranslationUnit(transUnit);
	
//	free(cursor_tail);
//	free(char_tail);
//	free(current_token_tail);
	int tmp = 5;

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
