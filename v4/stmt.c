#include "lex.h"
#include "ast.h"
#include "decl.h"
#include "expr.h"
#include "stmt.h"

AstStatement ParseStatement(){
	
	AstStatement stmt;

	switch(current_token.kind){
		case TK_ID:
			// TODO
			LOG("%s\n", "parse ID stmt");
			ParseSimpleStatement();
			break;
		case TK_IF:
			// TODO
			LOG("%s\n", "parse if stmt");
			stmt = (AstStatement)ParseIfStatement();	
		case TK_RETURN:
			// TODO
			LOG("%s\n", "parse return stmt");
			stmt = (AstStatement)ParseReturnStatement();	
			break;
		default:
			LOG("%s\n", "parse default stmt");
			stmt = (AstStatement)ParseCompoundStatement();	
			break;
	}

	return stmt;
}

AstStatement ParseSimpleStatement(){
	AstStatement stmt;

	ParseExpression();

	return stmt;	

}


/**
 * IfStmt = "if" [ SimpleStmt ";" ] Expression Block [ "else" ( IfStmt | Block ) ] .
 * SimpleStmt = EmptyStmt | ExpressionStmt | SendStmt | IncDecStmt | Assignment | ShortVarDecl .
 * ExpressionStmt = Expression .
 * Block是CompoundStatement
 */
AstIfStatement ParseIfStatement(){
	AstIfStatement stmt;

	EXPECT(TK_IF);
	// todo 使用了StartPeekToken而没有使用EndPeekToken，会有问题吗？
	// 处理[ SimpleStmt ";" ]
	StartPeekToken;
	ParseSimpleStatement();
	if(current_token.kind == TK_SEMICOLON){
		EXPECT(TK_SEMICOLON);
	}else{
		EndPeekToken;
	}
	
	ParseExpression();

	EXPECT(TK_LBRACE);	
	ParseCompoundStatement();
	EXPECT(TK_RBRACE);	

	// 处理[ "else" ( IfStmt | Block ) ]
	if(current_token.kind == TK_ELSE){		
		NEXT_TOKEN;
		if(current_token.kind == TK_IF){		
			ParseIfStatement();
		}else{
			EXPECT(TK_LBRACE);	
			ParseCompoundStatement();
			EXPECT(TK_RBRACE);	
		}
	}

	return stmt;
}
	
AstCompoundStatement ParseCompoundStatement(){
	// 找这个函数和{的代号，花了点时间。若经常用，花点时间记下来是不是能加快速度？
	// EXPECT(TK_LBRACE);	

	AstCompoundStatement compoundStmt;	
	CREATE_AST_NODE(compoundStmt, CompoundStatement); 
	// 处理函数体中的声明。
	// AstDeclarator decHead = NULL;	
	AstDeclarator decHead;	
	CREATE_AST_NODE(decHead, Declarator);
	decHead->next = NULL;
	AstDeclarator preDec = NULL;	
	AstDeclarator curDec;	
	// 当前token是声明字符集&&不是结束符&&不是}
	while(CurrentTokenIn(FIRST_Declaration) && (current_token.kind != TK_RBRACE)){
		curDec = declaration();
		if(preDec == NULL){
			preDec = curDec;
		}else{
			preDec->next = (AstNode)curDec;	
			preDec = curDec;
		}

		if(decHead->next == NULL){
			decHead->next = (AstNode)curDec;
		}
	}
	compoundStmt->decls = decHead->next;

	// 处理函数体中的语句。
	AstStatement headStmt;
	CREATE_AST_NODE(headStmt, Statement);
	headStmt->next = NULL;	
	// 当前token属于语句字符集 && 不是结束符 && 不是}
	AstStatement preStmt;
	AstStatement curStmt;
	while(current_token.kind != TK_RBRACE){
		//if(current_token.kind == TK_RBRACE){
		//	NEXT_TOKEN;
		//	break;
		//}
		// todo 未实现。
		curStmt = ParseStatement();
		if(current_token.kind == TK_RBRACE){
			// todo 这里很麻烦，符合语句的花括号要在ParseCompoundStatement外处理。
	//		NEXT_TOKEN;
			break;
		}
		if(preStmt == NULL){
			preStmt = curStmt;
		}else{
			preStmt->next = (AstNode)curStmt;
			preStmt = curStmt;
		}

		if(headStmt->next == NULL){
			headStmt->next = (AstNode)curStmt;
		}
	}
	compoundStmt->stmts = headStmt->next;

//	NEXT_TOKEN;

//	EXPECT(TK_RBRACE);	

	return compoundStmt;
}

AstReturnStatement ParseReturnStatement(){
	LOG("%s\n", "Parse return stmt");
	NEXT_TOKEN;
	NEXT_TOKEN;
//	NEXT_TOKEN;
}
