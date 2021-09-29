#include "lex.h"
#include "expr.h"

AstNodePtr primary_expr()
{
	TokenKind kind = current_token.kind;
	if(kind == TK_ID || kind == TK_NUM){
		Value value;
		// todo value设置什么值比较好？
		memset(&value, 0, sizeof(value));
		snprintf(value.value_str, 15, "t%d", new_tmp()); 
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
		snprintf(value.value_str, 15, "t%d", new_tmp()); 
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
		snprintf(value.value_str, 15, "t%d", new_tmp()); 
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
		snprintf(value.value_str, 15, "t%d", new_tmp()); 
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

int is_arithmetic_node(AstNodePtr node)
{
	TokenKind op = node->op;
	return op == TK_ADD || op == TK_MINUS || op == TK_MUL || op == TK_DIV;
}

void print_token(AstNodePtr node)
{
	if(node->op == TK_NUM){
		printf("\t%d\n", node->value.value_num);
	}else{
		printf("\t%s\n", node->value.value_str);
	}
}

void visit_arithmetic_node(AstNodePtr pNode)
{
	if(pNode && is_arithmetic_node(pNode)){
		visit_arithmetic_node(pNode->kids[0]);
		visit_arithmetic_node(pNode->kids[1]);
		if(pNode->kids[0] && pNode->kids[1]){
			// 补充漏掉的一个条件
			printf("\t%s = ", pNode->value.value_str);
			// todo 很难写出来。因为我在解析源代码时漏掉了一些节点创建。
			// todo 两个进行运算的元素是字符串还是数字，我为此纠结。
			// 有啥好纠结的？处理一下不就行了？
			if(pNode->kids[0]->op == TK_NUM){
				printf("\t%d ", pNode->kids[0]->value.value_num);
			}else{
				printf("\t%s ", pNode->kids[0]->value.value_str);
			}

			// 输出运算符号，例如：+。
			char token_name[20];
			get_token_name(token_name, pNode->op);
			printf("%s", token_name);

			if(pNode->kids[1]->op == TK_NUM){
				printf("\t%d ", pNode->kids[1]->value.value_num);
			}else{
				printf("\t%s ", pNode->kids[1]->value.value_str);
			}

			printf("\n");
		}
	}
}

int new_tmp()
{
	static int tmp_counter;
	return tmp_counter++;
}
