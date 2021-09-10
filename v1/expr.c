#include "lex.h"
#include "expr.h"


// todo 不知道怎么写
void primary_expr()
{
	TokenKind kind = current_token.kind;
	printf("kind = %d\n", kind);
	if(kind == TK_ID || kind == TK_NUM){
	
		// todo 创建节点
		
		NEXT_TOKEN;

	}else if(kind == TK_LPARENTHESES){
		NEXT_TOKEN;
		expression();
		assert(current_token.kind == TK_RPARENTHESES);
		EXPECT(TK_RPARENTHESES);
	}else{
		printf("expect id,num or (\n");
		exit(-3);
	}
}

// 解析乘法表达式
void mul_expr()
{
	primary_expr();
	TokenKind kind = current_token.kind;
	if(kind == TK_MUL || kind == TK_DIV){
		NEXT_TOKEN;
		mul_expr();
	}
}

// 解析加法表达式
void add_expr()
{
	// 使用右递归
	mul_expr();
	//EXPECT(TK_);
	TokenKind kind = current_token.kind;
	if(kind == TK_ADD || kind == TK_MINUS){
		NEXT_TOKEN;
		add_expr();
	}
}

// 解析表达式
// todo 不想现在设计AST结点，不设计返回值
void expression()
{
	add_expr();
}
