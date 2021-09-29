#include "decl.h"

AstNodePtr direct_declarator(){
	AstNodePtr decl = NULL;
	if(current_token.kind == TK_ID){
		Value value = current_token.value;
		decl = create_ast_node(TK_ID, value, NULL, decl);
		NEXT_TOKEN;	
	}else if(current_token.kind == TK_LPARENTHESES){
		NEXT_TOKEN;
		decl = declarator();
		EXPECT(TK_RPARENTHESES);
	}	// todo else是必须的吗？
	
	return decl;
}

AstNodePtr postfix_declarator(){
	AstNodePtr decl = direct_declarator();
	while(1){
		if(current_token.kind == TK_LPARENTHESES){
			NEXT_TOKEN;
			Value value;
			memset(&value, 0, sizeof(value));
			get_token_name(value.value_str, TK_FUNCTION);
			AstNodePtr *params;
			decl = create_ast_node(TK_FUNCTION, value, NULL, decl);
			params = &(decl->kids[0]);
			while(current_token.kind == TK_INT){
				Value value = current_token.value;
				*params = create_ast_node(TK_INT, value, NULL, NULL);
				// params = &(params->kids[0]);
				params = &((*params)->kids[0]);
				NEXT_TOKEN;
				if(current_token.kind == TK_COMMA){
					NEXT_TOKEN;
				}
			}	
			EXPECT(TK_RPARENTHESES);
		}else if(current_token.kind == TK_LBRACKET){
			NEXT_TOKEN;
			Value value = current_token.value;
			decl = create_ast_node(TK_ARRAY, value, NULL, decl);
			// NEXT_TOKEN;
			if(current_token.kind == TK_NUM){
				// todo value是当前token的值
				decl->value.value_num = value.value_num;
			}else{
				// todo value是0
				decl->value.value_num = 0;
			}
			NEXT_TOKEN;
			EXPECT(TK_RBRACKET);
		}else{  // todo else 是必须的吗？
			break;
		}
	}

	return decl;
}

AstNodePtr declarator(){
	if(current_token.kind == TK_MUL){
		Value value = current_token.value;
		NEXT_TOKEN;
		AstNodePtr decl = create_ast_node(TK_POINTER, value, NULL, NULL);
		decl->kids[1] = declarator();
		return decl;
	}
	return postfix_declarator();
}


AstNodePtr declaration(){
	TokenKind kind = current_token.kind;
	Value value = current_token.value;
	AstNodePtr decl;
	if(kind == TK_INT){
		NEXT_TOKEN;
		// 参考了作者的写法。我的写法是下面被注释的语句。
		decl = create_ast_node(kind, value, NULL, NULL);
		decl->kids[1] = declarator();
//		AstNodePtr decl = declarator();
		EXPECT(TK_SEMICOLON);
//		expr_stmt = create_ast_stmt_node(kind);
//		expr_stmt->expr = right;
	}else{
		EXPECT(TK_INT);
	}

	return decl;
}

void visit_declaration(AstNodePtr pNode){
	if(pNode){
		visit_declaration(pNode->kids[1]);
		// 为什么能写出这么多case？我记得那个复杂的例子。
		switch(pNode->op){
			case TK_ID:
				printf("%s is", pNode->value.value_str);
				break;
			case TK_ARRAY:
				printf("\t[%d] of", pNode->value.value_num);
				break;
			case TK_INT:
				printf("\tint\n");
				break;
			case TK_FUNCTION:
				printf("\tfunction(");
				AstNodePtr curParam = pNode->kids[0];
				while(curParam){
					// printf("%s", curParam->op);	
					char token_name[10];
					get_token_name(token_name, curParam->op);
					printf("%s", token_name);	
					curParam = curParam->kids[0];
					if(curParam){
						printf(",");
					}
				}
				printf(") which returns to");
				break;
			case TK_POINTER:
				printf("\tpointer to");
				break;
			default:
				printf("visit_declaration error\n");
				break;
		}
	}	
}
