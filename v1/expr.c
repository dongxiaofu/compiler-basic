#include "lex.h"
#include "expr.h"

AstNodePtr primary_expr()
{
	TokenKind kind = current_token.kind;
	if(kind == TK_ID || kind == TK_NUM){
		Value value;
		// todo value设置什么值比较好？
		memset(&value, 0, sizeof(value));
		value = current_token.value;
		NEXT_TOKEN;
		// todo 这样给value赋值可以吗？
		AstNodePtr expr = create_ast_node(kind, value, NULL, NULL);
// todo 在外面获取下一个token。
//		NEXT_TOKEN;
		return expr;
	}else if(kind == TK_LPARENTHESES){
		NEXT_TOKEN;
		// todo 不知道应该存储在kids的第0个还是第1个元素。
		AstNodePtr right  = expression();
		assert(current_token.kind == TK_RPARENTHESES);
		EXPECT(TK_RPARENTHESES);
		Value value;
		// todo value设置什么值比较好？
		memset(&value, 0, sizeof(value));
		AstNodePtr expr = create_ast_node(kind, value, NULL, right);
		return expr;
	}else{
		printf("expect id,num or (\n");
		exit(-3);
	}
}

// 解析乘法表达式
AstNodePtr mul_expr()
{
	AstNodePtr left = primary_expr();
	TokenKind kind = current_token.kind;
	AstNodePtr expr;
	if(kind == TK_MUL || kind == TK_DIV){
		Value value;
		// todo value设置什么值比较好？
		memset(&value, 0, sizeof(value));
		expr = create_ast_node(kind, value, left, NULL);
		NEXT_TOKEN;
		AstNodePtr right = mul_expr();
		expr->kids[0] = left;
		expr->kids[1] = right;
		return expr;
	}else{
		return left;
	}
}

// 解析加法表达式
AstNodePtr add_expr()
{
	// 使用右递归
	AstNodePtr left = mul_expr();
	TokenKind kind = current_token.kind;
	if(kind == TK_ADD || kind == TK_MINUS){
		Value value;
		// todo value设置什么值比较好？
		memset(&value, 0, sizeof(value));
		AstNodePtr expr = create_ast_node(kind, value, left, NULL);
		NEXT_TOKEN;
		AstNodePtr right  = add_expr();
		expr->kids[0] = left;
		expr->kids[1] = right;
		return expr;	
	}else{
		return left;
	}
}

// 解析表达式
AstNodePtr expression()
{
	return add_expr();
}

AstNodePtr create_ast_node(TokenKind op, Value value, AstNodePtr left, AstNodePtr right)
{
	AstNodePtr ast_node = (AstNodePtr)malloc(sizeof(struct astNode));
	memset(ast_node,0,sizeof(*ast_node));
	ast_node->op = op;
	ast_node->value = value;
	ast_node->kids[0] = left;
	ast_node->kids[1] = right;
	return ast_node;
}
