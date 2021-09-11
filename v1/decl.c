#include "decl.h"

void direct_declarator(){
	if(current_token.kind == TK_ID){
		NEXT_TOKEN;	
	}else if(current_token.kind == TK_LPARENTHESES){
		NEXT_TOKEN;
		declarator();
		EXPECT(TK_RPARENTHESES);
	}	// todo else是必须的吗？
}

void postfix_declarator(){
	direct_declarator();
	while(1){
		if(current_token.kind == TK_LPARENTHESES){
			NEXT_TOKEN;
			while(current_token.kind == TK_INT){
				NEXT_TOKEN;
				if(current_token.kind == TK_COMMA){
					NEXT_TOKEN;
				}
			}	
			EXPECT(TK_RPARENTHESES);
		}else if(current_token.kind == TK_LBRACKET){
			NEXT_TOKEN;
			if(current_token.kind == TK_NUM){
				// todo value是当前token的值
			}else{
				// todo value是0
			}
			NEXT_TOKEN;
			EXPECT(TK_RBRACKET);
		}else{  // todo else 是必须的吗？
			break;
		}
	}
}


void declarator(){
	if(current_token.kind == TK_MUL){
		NEXT_TOKEN;
		declarator();
		return;
	}
	postfix_declarator();
}


void declaration(){
	if(current_token.kind == TK_ID){
		NEXT_TOKEN;
		EXPECT(TK_ASSIGN);
		expression();
		EXPECT(TK_SEMICOLON);
	}else if(current_token.kind == TK_INT){
		NEXT_TOKEN;
		declarator();
		EXPECT(TK_SEMICOLON);
	}else{
		EXPECT(TK_INT);
	}
}
