#include "lex.h"
#include "stmt.h"
#include "expr.h"
#include "decl.h"

int main(int argc, char *argv[])
{
	fp = fopen("test.c", "r");
	if(fp == NULL){
		perror("open test.c error");
		exit(-1);
	}

	NEXT_TOKEN;
	compound_stmt();

	return 0;
}

void compound_stmt()
{
//	NEXT_TOKEN;
	assert(current_token.kind == TK_LBRACE);
	// 希望是{
	EXPECT(TK_LBRACE);
	// 中间是其他语句，把它们放在一个单链表中。
	while(check_is_prefix_of_stmt(current_token.kind) == 1){
		stmt();
	}
	// 希望是}
	assert(current_token.kind == TK_RBRACE);
	EXPECT(TK_RBRACE);

}

void stmt()
{
	TokenKind kind = current_token.kind;

	switch(kind){
		case TK_IF:
			// 解析IF
			if_stmt();
			break;
		case TK_WHILE:
			// 解析while
			while_stmt();
			break;
		case TK_LBRACE:
			// 解析复合语句
			compound_stmt();
			break;
		case TK_ID:
		case TK_INT:
			// 解析复合语句
			expr_stmt();
			break;
		default:
			// todo 其他，待细化
			default_stmt();
			break;
	}
}

// 解析if语句
void if_stmt()
{
	printf("parse if statement\n");
	assert(current_token.kind == TK_IF);
	EXPECT(TK_IF);
	assert(current_token.kind == TK_LPARENTHESES);
	EXPECT(TK_LPARENTHESES);
	// 处理表达式
	expression();
	assert(current_token.kind == TK_RPARENTHESES);
	EXPECT(TK_RPARENTHESES);
	// 解析复合语句
	compound_stmt();
}
// 解析while语句
void while_stmt()
{
	printf("parse while statement\n");
	assert(current_token.kind == TK_WHILE);
	EXPECT(TK_WHILE);	
	EXPECT(TK_LPARENTHESES);
	// 处理表达式
	expression();
	assert(current_token.kind == TK_RPARENTHESES);
	EXPECT(TK_RPARENTHESES);
	// 解析复合语句
	compound_stmt();
	
}

void default_stmt()
{
	printf("parse default statement\n");
	do{
		NEXT_TOKEN;
	}while(current_token.kind != TK_RBRACE);
}

void expr_stmt()
{
	// 只能解析 a = 5; 
	if(current_token.kind == TK_ID){
		NEXT_TOKEN;
		assert(current_token.kind == TK_ASSIGN);
		EXPECT(TK_ASSIGN);
		expression();
		assert(current_token.kind == TK_SEMICOLON);
		EXPECT(TK_SEMICOLON);
	}else if(current_token.kind == TK_INT){
		
		printf("process int\n");		
		declaration();	

	}else{
		printf("expr stmt error\n");
	}

}

int check_is_prefix_of_stmt(TokenKind kind)
{
	int count = sizeof(prefix_of_stmt_token_kinds) / sizeof(prefix_of_stmt_token_kinds[0]);
	for(int i = 0; i < count; i++){
		if(kind == prefix_of_stmt_token_kinds[i]){
			return 1;
		}
	}
	return 0;
}

