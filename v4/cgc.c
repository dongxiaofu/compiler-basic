#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "input.h"
#include "decl.h"
#include "lex.h"
#include "ast.h"
#include "packages.h"

int main(int argc, char *argv[])
{
	printf("I am a scanner\n");

//	printf("argc = %d\n", argc);
//	for(int i = 0; i < argc; i++){
//		printf("argv[%d] = %s\n", i, argv[i]);
//	}

	if(argc < 2){
		ERROR("need filename");
		exit(-1);
	}	

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
//	while(1){
//		if(++loop_counter > 80){
//			break;
//		}
//		get_token();
//	}
//	printf("scan token over\n");
//	exit(3);
	
//	while(1){
		get_token();
	//	StartPeekToken();
	//	get_token();
	//	get_token();
	//	get_token();
	//	get_token();
	//	get_token();
	//	get_token();
	//	EndPeekToken();
		if(current_token.kind == TK_EOF) return 0;
		ParseTranslationUnit();
//	}

	
	free(cursor_tail);
	free(char_tail);
	free(current_token_tail);

	return 0;
}

AstTranslationUnit ParseTranslationUnit(){
	
	AstTranslationUnit *p;
	// CREATE_AST_NODE(p, "TranslationUnit");
	CREATE_AST_NODE(p, TranslationUnit);
	ParseSourceFile();
	LOG("parse unit over\n");

	return *p;
//	ParseExpression();

//	return *p;

	while(current_token.kind != TK_EOF){
		// 测试 start
//		ParseCompositeLit();
		ParseExpression();
//		declaration();
		// ParseType();
		continue;
		// 测试 end
		if(current_token.kind == TK_FUNC){
			LOG("parse func\n");
			p->extDecls = declaration();
	
		}else{
			LOG("parse decl\n");
			p->extDecls = declaration();
		}
		LOG("parse loop\n");
	}

	LOG("parse unit over\n");
}
