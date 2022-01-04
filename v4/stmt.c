#include "lex.h"
#include "ast.h"
#include "decl.h"
#include "expr.h"
#include "stmt.h"

AstStatement ParseStatement(){
	
	AstStatement stmt;

	switch(current_token.kind){
//		case TK_ID:
//			// TODO
//			LOG("%s\n", "parse ID stmt");
//			// stmt = ParseSimpleStatement();
//			stmt = ParseLabelStatement();
//			break;
		case TK_IF:
			// TODO
			LOG("%s\n", "parse if stmt");
			stmt = (AstStatement)ParseIfStatement();	
			break;
		case TK_RETURN:
			// TODO
			LOG("%s\n", "parse return stmt");
			stmt = (AstStatement)ParseReturnStatement();	
			break;
		case TK_SELECT:
			// TODO
			LOG("%s\n", "parse select stmt");
			stmt = (AstStatement)ParseSelectStmt();	
			break;
		case TK_SWITCH:
			LOG("%s\n", "parse switch stmt");
			stmt = (AstStatement)ParseSwitchStmt();	
			break;
		case TK_FOR:
			LOG("%s\n", "parse for stmt");
			stmt = (AstStatement)ParseForStmt();	
			break;
		case TK_GO:
			LOG("%s\n", "parse go stmt");
			stmt = (AstStatement)ParseGoStmt();	
			break;
		case TK_GOTO:
			LOG("%s\n", "parse goto stmt");
			stmt = (AstStatement)ParseGotoStatement();	
			break;
		case TK_CONTINUE:
			LOG("%s\n", "parse continue stmt");
			stmt = (AstStatement)ParseContinueStatement();	
			break;
		case TK_FALLTHROUGH:
			LOG("%s\n", "parse Fallthrough stmt");
			stmt = (AstStatement)ParseFallthroughStmt();	
			break;
		case TK_DEFER:
			LOG("%s\n", "parse Defer stmt");
			stmt = (AstStatement)ParseDeferStmt();
			break;
		case TK_LBRACE:
			LOG("%s\n", "parse block");
			stmt = (AstStatement)ParseFunctionBody();
			break;
		case TK_BREAK:
			LOG("%s\n", "parse break");
			stmt = (AstStatement)ParseBreakStmt();
			break;
		default:
			LOG("%s\n", "parse default stmt");
			stmt = (AstStatement)ParseCompoundStatement();	
			break;
	}

	return stmt;
}

/**
 * LabeledStmt = Label ":" Statement .
Label       = identifier .
 */
AstStatement ParseLabelStatement(){
	ParseIdentifier();
	NEXT_TOKEN;
	ParseStatement();

 	AstStatement stmt;
 	CREATE_AST_NODE(stmt, Statement);
 	return stmt;
}

// /**
//  * LabeledStmt = Label ":" Statement .
// Label       = identifier .
//  */
// AstStatement ParseLabelStatement(){
// 	// 判断是不是赋值语句。
// 	char assign_flag = 0;
// 	StartPeekToken();
// 	while(1){
// 		NO_TOKEN;
// 		// if(current_token.kind == TK_EQUAL){
// 		// IsAssignOp
// 		// if(current_token.kind == TK_ASSIGN){
// 		if(IsAssignOp(current_token.kind) == 1){
// 			assign_flag = 1;
// 			break;
// 		}
// 		NEXT_TOKEN;
// 	}
// 	EndPeekToken();
// 
// 	if(assign_flag == 1){
// 		ParseAssignmentsStmt();
// 	}else{
// 		ParseSimpleStatement();
// 	}
// 
// 	AstStatement stmt;
// 	CREATE_AST_NODE(stmt, Statement);
// 	return stmt;
// }

/**
 * SimpleStmt = EmptyStmt | ExpressionStmt | SendStmt | IncDecStmt | Assignment | ShortVarDecl .
 *
 */
AstStatement ParseSimpleStatement(){
	AstStatement stmt;

//	int type = -1;		// EmptyStmt
//	StartPeekToken();
//	// TODO 很低效的方法。想不到其他更好的方法。
//	//while(1){
//	//分析SimpleStmt出现的地方总结出这个结论。
//	while(current_token.kind != TK_SEMICOLON && current_token.kind != TK_LBRACE){
//		NO_TOKEN;
//		if(current_token.kind == TK_INIT_ASSIGN){
//			type = 1;	// ShortVarDecl
//			break;
//		}else if(current_token.kind == TK_RECEIVE){
//			type = 2;	// SendStmt
//			break;
//		}else if(current_token.kind == TK_DEC || current_token.kind == TK_INC){
//			type =3 ;	// IncDecStmt
//			break;
//		}else if(current_token.kind == TK_ASSIGN){
//			type = 4;	// Assignment
//			break;
//		}else{
//			type = 5;	// ExpressionStmt
//		}
//
//		NEXT_TOKEN;
//	}
//	EndPeekToken();

//	int type = -1;		// EmptyStmt
//	StartPeekToken();
//	if(current_token.kind == TK_ID){
//		type = 1;		// ShortVarDecl
//		NEXT_TOKEN;
//		if(current_token.kind == TK_ASSIGN){
//			type = 4; // Assignment
//		}else if(IsAddOp() == 1 || IsMulOp() == 1){
//			NEXT_TOKEN;
//			if(current_token.kind == TK_ASSIGN){
//				type = 4; // Assignment
//			}
//		} 
//	}else{
//		type = 5;	// ExpressionStmt
//		ParseExpression();
//		if(current_token.kind == TK_INC || current_token.kind == TK_DEC){
//			type =3 ;	// IncDecStmt
//		}else if(current_token.kind == TK_RECEIVE){
//			type = 2;	// SendStmt
//		}else if(current_token.kind == TK_ASSIGN){
//			type = 4;	// Assignment	
//		}else if(IsAddOp() == 1 || IsMulOp() == 1){
//			NEXT_TOKEN;
//			if(current_token.kind == TK_ASSIGN){
//				type = 4; // Assignment
//			}
//		} 
//	}	
//	EndPeekToken();

	int type = -1;		// EmptyStmt
	if(current_token.kind == TK_BREAK){
//		goto start;
	}
	// ShortVarDecl = IdentifierList ":=" ExpressionList .
	if(current_token.kind == TK_ID){
		StartPeekToken();
		NEXT_TOKEN;
		if(current_token.kind == TK_INIT_ASSIGN){
			type = 1;
			EndPeekToken();
			goto start;
		}else if(current_token.kind == TK_COMMA){
			NEXT_TOKEN;
			ParseIdentifierList();
			if(current_token.kind == TK_INIT_ASSIGN){
				type = 1;
				EndPeekToken();
				goto start;
			}
		}
		EndPeekToken();	
	}

	StartPeekToken();
	if(CurrentTokenIn(FIRST_Expression) == 1){
		type = 5;	// ExpressionStmt
		if(current_token.kind == TK_ADD || current_token.kind == TK_MINUS || current_token.kind == TK_MUL){
//			goto start;
		}
		ParseExpression();
		if(current_token.kind == TK_INC || current_token.kind == TK_DEC){
			type =3 ;	// IncDecStmt
		}else if(current_token.kind == TK_RECEIVE){
			type = 2;	// SendStmt
		}else if(current_token.kind == TK_ASSIGN){
			type = 4;	// Assignment	
		// }else if(IsAddOp() == 1 || IsMulOp() == 1){
		}else if(TK_ASSIGN <= current_token.kind && current_token.kind <= TK_BIT_CLEAR_ASSIGN){
			type = 4; // Assignment
		}else if(current_token.kind == TK_COMMA){
			NEXT_TOKEN;
			ParseExpressionList();
			if(current_token.kind == TK_ASSIGN){
				type = 4; // Assignment
			}
		} 
	}	
	EndPeekToken();	

start:
	printf("【type = %d】\n", type);
	if(type == 1){
		stmt = (AstStatement)ParseShortVarDecl();
	}else if(type == 2){
		stmt = (AstStatement)ParseSendStmt();
	}else if(type == 3){
		stmt = (AstStatement)ParseIncDecStmt();
	}else if(type == 4){
		stmt = (AstStatement)ParseAssignmentsStmt();
	}else if(type == 5){
		stmt = (AstStatement)ParseExpression();
	}else if(type == 6){
		stmt = (AstStatement)ParseStatement();
	}else{
		CREATE_AST_NODE(stmt, Statement);
	}

	expect_semicolon;

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
	// TODO 要给stmt的成员赋值，必定需要先分配一片内存，否则，会造成段错误。
	CREATE_AST_NODE(stmt, IfStatement);

	printf("【if start】\n");
	EXPECT(TK_IF);
	// todo 使用了StartPeekToken而没有使用EndPeekToken，会有问题吗？
	// 处理[ SimpleStmt ";" ]
	AstStatement simpleStmt = NULL;
//	StartPeekToken;
//	simpleStmt = ParseSimpleStatement();
//	if(current_token.kind == TK_SEMICOLON){
//		EXPECT(TK_SEMICOLON);
//	}else{
//		EndPeekToken;
//		simpleStmt = NULL;
//	}

	// 处理[ SimpleStmt ";" ]
	char semicolon_flag = 0;
	StartPeekToken();
	while(current_token.kind != TK_LBRACE){
		NO_TOKEN;
		if(current_token.kind == TK_SEMICOLON){
			semicolon_flag = 1;
			break;
		}
		NEXT_TOKEN;
	}
	EndPeekToken();

	if(semicolon_flag == 1){
		simpleStmt = ParseSimpleStatement();
		NEXT_TOKEN;
	}

	stmt->simpleStmt = simpleStmt;	

	AstExpression expr = ParseExpression();
	stmt->expr = expr;

//	EXPECT(TK_LBRACE);	
	// AstCompoundStatement compoundStatement  = ParseCompoundStatement();
	AstCompoundStatement compoundStatement  = ParseFunctionBody();
//	EXPECT(TK_RBRACE);	
	stmt->thenStmt = (AstStatement)compoundStatement;

	AstStatement  elseStmt = NULL;
	// 处理[ "else" ( IfStmt | Block ) ]
	if(current_token.kind == TK_ELSE){		
		NEXT_TOKEN;
		if(current_token.kind == TK_IF){		
			elseStmt = ParseIfStatement();
		}else{
//			EXPECT(TK_LBRACE);	
			// elseStmt = ParseCompoundStatement();
			elseStmt = ParseFunctionBody();
//			EXPECT(TK_RBRACE);	
		}
	}

	stmt->elseStmt = (AstStatement)elseStmt;

	expect_semicolon;

	printf("【if end】\n");

	return stmt;
}

/**
 * Declaration | LabeledStmt | SimpleStmt
 */
AstCompoundStatement ParseCompoundStatement(){
	AstCompoundStatement compoundStmt;	
	CREATE_AST_NODE(compoundStmt, CompoundStatement); 

	if(CurrentTokenIn(FIRST_Declaration) == 1){
		declaration();
		return compoundStmt;
	}

	// type的值：0--LabeledStmt，1--SimpleStmt
	unsigned char type = 1;	
	StartPeekToken();
	if(current_token.kind == TK_ID){
		NEXT_TOKEN;
		if(current_token.kind == TK_COLON){
			type = 0;
		}
	}
	EndPeekToken();

	if(type == 0){
		ParseLabelStatement();
	}else{
		ParseSimpleStatement();
	}

	return compoundStmt;
}

// AstCompoundStatement ParseCompoundStatement(){
// 	// 找这个函数和{的代号，花了点时间。若经常用，花点时间记下来是不是能加快速度？
// 	// EXPECT(TK_LBRACE);	
// 
// 	AstCompoundStatement compoundStmt;	
// 	CREATE_AST_NODE(compoundStmt, CompoundStatement); 
// 	// 处理函数体中的声明。
// 	// AstDeclarator decHead = NULL;	
// 	AstDeclarator decHead;	
// 	CREATE_AST_NODE(decHead, Declarator);
// 	decHead->next = NULL;
// 	AstDeclarator preDec = NULL;	
// 	AstDeclarator curDec;	
// 	// 当前token是声明字符集&&不是结束符&&不是}
// 	while(CurrentTokenIn(FIRST_Declaration) && (current_token.kind != TK_RBRACE)){
// 		NO_TOKEN;
// 		curDec = declaration();
// 		if(preDec == NULL){
// 			preDec = curDec;
// 		}else{
// 			preDec->next = (AstNode)curDec;	
// 			preDec = curDec;
// 		}
// 
// 		if(decHead->next == NULL){
// 			decHead->next = (AstNode)curDec;
// 		}
// 	}
// //	return compoundStmt;
// 	compoundStmt->decls = decHead->next;
// //	return compoundStmt;
// 
// 	// 处理函数体中的语句。
// 	AstStatement headStmt;
// 	CREATE_AST_NODE(headStmt, Statement);
// 	headStmt->next = NULL;	
// 	// 当前token属于语句字符集 && 不是结束符 && 不是}
// 	AstStatement preStmt = NULL;
// 	AstStatement curStmt;
// 	// while(current_token.kind != TK_RBRACE){
// 	while(current_token.kind != TK_RBRACE && current_token.kind != TK_CASE && current_token.kind !=TK_DEFAULT){
// 		NO_TOKEN;
// 		//if(current_token.kind == TK_RBRACE){
// 		//	NEXT_TOKEN;
// 		//	break;
// 		//}
// 		// todo 未实现。
// 		curStmt = ParseStatement();
// //		if(current_token.kind == TK_RBRACE){
// //			// todo 这里很麻烦，复合语句的花括号要在ParseCompoundStatement外处理。
// //	//		NEXT_TOKEN;
// //			break;
// //		}
// 		if(preStmt == NULL){
// 			preStmt = curStmt;
// 		}else{
// 			preStmt->next = (AstNode)curStmt;
// 			preStmt = curStmt;
// 		}
// 
// 		if(headStmt->next == NULL){
// 			headStmt->next = (AstNode)curStmt;
// 		}
// 
// 		if(current_token.kind == TK_RBRACE){
// 			// todo 这里很麻烦，复合语句的花括号要在ParseCompoundStatement外处理。
// 	//		NEXT_TOKEN;
// 			break;
// 		}
// 	}
// 	compoundStmt->stmts = headStmt->next;
// 
// //	NEXT_TOKEN;
// 
// //	EXPECT(TK_RBRACE);	
// 
// 	return compoundStmt;
// }

/**
 * ReturnStmt = "return" [ ExpressionList ] .
 */
AstReturnStatement ParseReturnStatement(){
	LOG("%s\n", "Parse return stmt");
//	NEXT_TOKEN;
//	NEXT_TOKEN;
//	NEXT_TOKEN;
	if(current_token.kind == TK_RETURN){
		expect_token(TK_RETURN);
	}	

	if(current_token.kind != TK_RBRACE){
		ParseExpressionList();
	}	

	expect_semicolon;

	AstReturnStatement stmt;
	CREATE_AST_NODE(stmt, ReturnStatement);

	return stmt;
}

// BreakStmt = "break" [ Label ] .
AstBreakStatement ParseBreakStatement(){
	NEXT_TOKEN;
	AstBreakStatement breakStmt;
	CREATE_AST_NODE(breakStmt, BreakStatement);
	// todo 内存泄露，但暂时没有找到好的处理方法。
	char *label = (char *)malloc(sizeof(char)*MAX_NAME_LEN);
	AstDeclarator decl = ParseIdentifier();	
	if(decl != NULL){
		strcpy(label, decl->id);
	}else{
		label = NULL;
	}
	
	breakStmt->label = label;

	return breakStmt;
}

// 和ParseBreakStatement代码完全相同，寻机优化。
// ContinueStmt = "continue" [ Label ] .
AstContinueStatement ParseContinueStatement(){
	NEXT_TOKEN;
	expect_semicolon;
	AstContinueStatement continueStmt;
	CREATE_AST_NODE(continueStmt, ContinueStatement);
	// todo 内存泄露，但暂时没有找到好的处理方法。
	char *label = (char *)malloc(sizeof(char)*MAX_NAME_LEN);
	AstDeclarator decl = ParseIdentifier();	
	expect_semicolon;
	if(decl != NULL){
		strcpy(label, decl->id);
	}else{
		label = NULL;
	}
	
	continueStmt->label = label;

	return continueStmt;
}

// GotoStmt = "goto" Label .
AstGotoStatement ParseGotoStatement(){
	NEXT_TOKEN;
	AstGotoStatement gotoStmt;
	CREATE_AST_NODE(gotoStmt, GotoStatement);
	// todo 内存泄露，但暂时没有找到好的处理方法。
	char *label = (char *)malloc(sizeof(char)*MAX_NAME_LEN);
	AstDeclarator decl = ParseIdentifier();	
	expect_semicolon;
	if(decl != NULL){
		strcpy(label, decl->id);
	}else{
		label = NULL;
	}
	
	gotoStmt->label = label;

	return gotoStmt;
}

// todo 难处理，没有思路，先搁置。
/**
 * RecvStmt   = [ ExpressionList "=" | IdentifierList ":=" ] RecvExpr .
RecvExpr   = Expression .
 */
AstRecvStmt ParseRecvStmt(){
	// 先识别<-前面的token是不是=或:=。
//	StartPeekToken();	
//	char type = -1;
//	char flag_less = 0;
//	int tokens[200];
//	int i = -1;
//	int flag_chan = 0;
//	int pre_token = -1;
	// TODO 如果没有<-，就一直循环下去吗？效率太低，想办法优化。
//	while(current_token.kind != TK_LESS || flag_less == 0){
//		NO_TOKEN;
//		tokens[++i] = current_token.kind;
//		pre_token = current_token.kind;
//		NEXT_TOKEN;
//		if(pre_token == TK_LESS && current_token.kind == TK_MINUS){
//			flag_chan = 1;
//			tokens[++i] = current_token.kind;
//			break;
//		}else{
//			pre_token = -1;
//		}
//	}

//	char type = -1;
//	if(flag_chan == 1){
//		 if(i > 2){
//			if(tokens[i-2] == TK_EQUAL){
//				type = 1;
//				if(tokens[i-3] == TK_COLON){
//					type = 2;
//				}
//			}
//		}
//	}

	StartPeekToken();	
	char type = -1;
	int flag_chan = 0;

	while(current_token.kind != TK_COLON){
		NO_TOKEN;
	//	tokens[++i] = current_token.kind;
		if(current_token.kind == TK_RECEIVE){
			flag_chan  = 1;
		}else if(current_token.kind == TK_ASSIGN){
			type = 1;
		}else if(current_token.kind == TK_INIT_ASSIGN){
			type = 2;	
		}
		NEXT_TOKEN;
	}
	
	EndPeekToken();
	
	if(flag_chan == 0){
		ERROR("%s\n", "Expect a <-");
		// TODO 寻机优化成在ERROR中退出。
		exit(2);
	}

	if(type == 1){
		ParseExpressionList();
	}else if(type == 2){
		ParseIdentifierList();
	}else{
		// TODO 什么都不需要做吗？
		// ERROR("%s\n", "Expect a RecvStmt");
	}	

	// 处理<-
//	expect_token(TK_LESS);
//	expect_token(TK_MINUS);
	EXPECT(TK_RECEIVE);
	// TODO 是sender还是receiver？
	AstExpression expr = ParseExpression();
	
	// TODO 解析完了RecvStmt，但没有建立AST。返回值是不正确的。
	return expr;
}

// todo 很好解析SendStmt，但是非常不容易识别这种表达式。
/**
 * SendStmt = Channel "<-" Expression .
   Channel  = Expression .
 */
AstSendStmt ParseSendStmt(){
	AstExpression channel = ParseExpression();

	EXPECT(TK_RECEIVE);

	AstExpression sender = ParseExpression();

	AstSendStmt sendStmt;
	CREATE_AST_NODE(sendStmt, SendStmt);
	sendStmt->channel = channel;
	sendStmt->expr = sender;

	return sendStmt;
}

// IncDecStmt = Expression ( "++" | "--" ) .
AstIncDecStmt ParseIncDecStmt(){
	AstIncDecStmt stmt;
	CREATE_AST_NODE(stmt, IncDecStmt);
	// todo 要处理expr是空的情况吗？
	AstExpression expr = ParseExpression();
//	StartPeekToken();	
//	if(current_token.kind != TK_INC && current_token.kind != TK_DEC){
//		EndPeekToken();
//		expect_token(TK_INC);
		if(current_token.kind == TK_INC || current_token.kind == TK_DEC){
			NEXT_TOKEN;
		}else{
			expect_token(TK_INC);
		}
//	}
	stmt->expr = expr;
	stmt->op = current_token.kind;	
	return stmt;
}

/**
 * LabeledStmt = Label ":" Statement .
   Label       = identifier .
 */
// TODO 有没有和 ParseLabelStatement 重复？
AstLabeledStmt ParseLabeledStmt(){
	char *label = (char *)malloc(sizeof(char)*MAX_NAME_LEN);
	AstDeclarator decl = ParseIdentifier();	
	if(decl != NULL){
		strcpy(label, decl->id);
	}else{
		label = NULL;
	}
	// todo label是不能为空的。若为空，此处如何处理？
	// TK_COLON 是冒号。
	expect_token(TK_COLON);
	AstStatement stmt = ParseStatement();
	
	AstLabeledStmt labelStmt;
	CREATE_AST_NODE(labelStmt, LabelStmt);
	labelStmt->label = label;
	labelStmt->stmt = stmt;

	return labelStmt;
}

// DeferStmt = "defer" Expression .
AstDeferStmt ParseDeferStmt(){
	NEXT_TOKEN;
	ParseExpression();
	// TODO 分号在表达式中解析还是在语句中解析？
	expect_semicolon;
	AstDeferStmt stmt;
	CREATE_AST_NODE(stmt, DeferStmt);
	stmt->stmt = stmt;

	return stmt;
}
 
// GoStmt = "go" Expression .
AstGoStmt ParseGoStmt(){
	NEXT_TOKEN;
	AstGoStmt stmt;
	CREATE_AST_NODE(stmt, GoStmt);
	stmt->expr = ParseExpression();
	expect_semicolon;

	return stmt;
}

// FallthroughStmt = "fallthrough" .
AstFallthroughStmt ParseFallthroughStmt(){
	NEXT_TOKEN;
	expect_semicolon;
	AstFallthroughStmt stmt;
	CREATE_AST_NODE(stmt, FallthroughStmt);

	return stmt;
}

// todo 未实现。
// 要从两个运算符中提取出第一个运算符。这不是难事。
/**
assign_op = [ add_op | mul_op ] "=" .
add_op     = "+" | "-" | "|" | "^" .
mul_op     = "*" | "/" | "%" | "<<" | ">>" | "&" | "&^" .
 */
int ParseAssignmentsOp(){
	// int op = TK_ASSIGN;
	// // 除等号外是否有其他符号。 
	// char has_other_op = 1;
	// if(IsMulOp() == 1){
	// 	op = ParseMulOp();
	// }else if(IsAddOp() == 1){
	// 	op = ParseAddOp();
	// }else{
	// 	has_other_op = 0;
	// 	expect_token(TK_ASSIGN);	
	// }
	// 
	// if(has_other_op == 1){
	// 	expect_token(TK_ASSIGN);
	// }

	int op = current_token.kind;
	NEXT_TOKEN;	

	return op;
}

int IsAssignOp(int token_kind){
	return (TK_ASSIGN <= token_kind && token_kind <= TK_BIT_CLEAR_ASSIGN);
}

/**
 * Assignment = ExpressionList assign_op ExpressionList .
assign_op = [ add_op | mul_op ] "=" .
add_op     = "+" | "-" | "|" | "^" .
mul_op     = "*" | "/" | "%" | "<<" | ">>" | "&" | "&^" .
 */
AstAssignmentsStmt ParseAssignmentsStmt(){
	
	AstExpression left_expr = ParseExpressionList();
	int op = ParseAssignmentsOp();
	AstExpression right_expr = ParseExpressionList();

	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	expr->op = op;
	expr->kids[0] = left_expr;
	expr->kids[1] = right_expr;

	AstAssignmentsStmt stmt;
	CREATE_AST_NODE(stmt, AssignmentsStmt);
	stmt->expr = expr;

	return stmt;
}


/**
 * SelectStmt = "select" "{" { CommClause } "}" .
CommClause = CommCase ":" StatementList .
CommCase   = "case" ( SendStmt | RecvStmt ) | "default" .
RecvStmt   = [ ExpressionList "=" | IdentifierList ":=" ] RecvExpr .
RecvExpr   = Expression .
 */ 
AstSelectStmt ParseSelectStmt(){
	AstSelectStmt selectStmt;
	CREATE_AST_NODE(selectStmt, SelectStmt);
	
	selectStmt->stmt = ParseAstSelectCaseStmt();
	
	return selectStmt;
}

// 获取语句的类型。1是SendStmt，2是RecvStmt，-1是其他。
int getStmtType(){
	int tokens[200];
	int i = -1;
	int receive_operator_index = -1;
	int assign_operator_index = -1;
	int assign_init_operator_index = -1;
	char stmt_type = -1;
	
	StartPeekToken();
	while(current_token.kind != TK_COLON){
		NO_TOKEN;
		tokens[++i] = current_token.kind;
		if(current_token.kind == TK_RECEIVE){
			receive_operator_index = i;
			break;
		}else if(current_token.kind == TK_ASSIGN){
			assign_operator_index = i;
		}else if(current_token.kind == TK_INIT_ASSIGN){
			assign_init_operator_index = i;
		}
		NEXT_TOKEN;
	}
	EndPeekToken();
	
	// TODO 下面的一系列代码写得太难看，寻机优化。
	// 没有<-，报错。
	if(receive_operator_index == -1){
		EXPECT(TK_RECEIVE);
	}	

	// case后面紧邻<-，一定是接收语句。
	if(receive_operator_index == 0){
		stmt_type = 2;
	}

	// = <- ch 这种，是接收语句。
	if(assign_operator_index != -1){
		stmt_type = 2;
	}

	// := <- ch 这种，是接收语句。
	if(assign_init_operator_index != -1){
		stmt_type = 2;
	}

	// SendStmt = Channel "<-" Expression .
	// Channel  = Expression .
	if(assign_operator_index == -1 && assign_init_operator_index == -1 && receive_operator_index > 0){
		stmt_type = 1;
	}
	
	return stmt_type;
}

// 获取语句的类型。1是SendStmt，2是RecvStmt，-1是其他。
// int getStmtType(){
// 
// 	char stmt_type = -1;
// 	char tokens[200];
// 	char flag = 0;
// 	int i = -1;
// 	StartPeekToken(); 
// 	while(1){
// 		NO_TOKEN;
// 		// tokens[i++] = current_token.kind;
// 		tokens[++i] = current_token.kind;
// 		if(flag == 1){
// 			if(current_token.kind == TK_MINUS){
// 				break;
// 			}else{
// 				flag == 0;
// 			}
// 		}
// 		if(flag == 0 && current_token.kind == TK_LESS){
// 			flag = 1;	
// 		}	
// 		NEXT_TOKEN;
// 	}
// 
// 	if(flag == 1 && i >= 2){
// 		int idx = i - 2;
// 		if(tokens[idx] == TK_EQUAL){
// 			stmt_type = 2;
// 		}else{
// 			stmt_type = 1;
// 		}
// 	}	
// 
// 	EndPeekToken(); 
// 
// 	return stmt_type;
// }

AstSelectCaseStatement ParseAstSelectCaseStmt(){
	// 跳过select
	NEXT_TOKEN;
	// 跳过{
	expect_token(TK_LBRACE);
	// 一个循环
	while(current_token.kind == TK_CASE || current_token.kind == TK_DEFAULT){
		// 跳过冒号
		expect_token(TK_COLON);
		// 处理"case" ( SendStmt | RecvStmt ) | "default" .
		if(current_token.kind == TK_DEFAULT){
			// do nothing
		}else{
			// 分辨是SendStmt还是RecvStmt
			char stmt_type = getStmtType();
			if(stmt_type == 1){
				ParseSendStmt();
			}else if(stmt_type == 2){
				ParseRecvStmt();
			}else{
				ERROR("%s\n", "Require a SendStmt or RecvStmt");
			}
		}
		
		// 解析StatementList。又是一个循环。
		// todo 分解问题，放到另外的函数处理吧。
		ParseStatementList();
	}
	// 跳过}
	expect_token(TK_RBRACE);
	expect_semicolon;
}

// TODO 建立单链表，重复了很多次的代码。
// StatementList = { Statement ";" } .
AstStatement ParseStatementList(){

	AstStatement preStmt = NULL;
	AstStatement headStmt;
	CREATE_AST_NODE(headStmt, Statement);
	headStmt->next = NULL;

	// while(current_token.kind != TK_SEMICOLON){
	// TODO 测试出来的。处理switch时需要这样做。非switch这样做有问题吗？
	while(current_token.kind != TK_SEMICOLON && current_token.kind != TK_CASE && current_token.kind != TK_DEFAULT){
		NO_TOKEN;	
		if(current_token.kind == TK_RBRACE){
			break;
		}

		AstStatement curStmt = ParseStatement();
		if(preStmt == NULL){
			preStmt = curStmt;
		}else{
			preStmt->next = curStmt;
			preStmt = curStmt;
		}

		if(headStmt->next == NULL){
			headStmt->next = curStmt;
		}

		// 断点调试出来的。
		if(current_token.kind == TK_CASE || current_token.kind == TK_DEFAULT){
			break;
		}
		// NEXT_TOKEN;
		// expect_token(TK_SEMICOLON);
		expect_semicolon;
	}

	return headStmt->next;
}

AstExpression ParseCondition(){
	AstExpression expr = ParseExpression();
	return expr;
}

/**
 * ForClause = [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] .
InitStmt = SimpleStmt .
PostStmt = SimpleStmt .
 */
AstStatement ParseForClause(){

	// TODO [ InitStmt ] 等都是可选的。此处的代码把它们按照必选处理。寻机再完善。
	ParseSimpleStatement();
	// expect_token(TK_SEMICOLON);
	expect_semicolon;
	ParseCondition();
	expect_semicolon;
	// expect_token(TK_SEMICOLON);
	ParseSimpleStatement();

	AstStatement stmt;
	return stmt;
}

AstExpression ParseRangeExpr(){

	AstExpression expr = ParseExpression();

	return expr;
}

/**
 * RangeClause = [ ExpressionList "=" | IdentifierList ":=" ] "range" Expression .
 */
AstStatement ParseRangeClause(){
	// 处理[ ExpressionList "=" | IdentifierList ":=" ]
	// 识别是表达式列表还是标识符列表。
//	char type = -1;
//	int i = -1;
//	int tokens[200];
//	StartPeekToken();
//	while(1){
//		NO_TOKEN;
//		tokens[++i] = current_token.kind;
//		if(current_token.kind == TK_RANGE){
//			break;
//		}	
//		NEXT_TOKEN;
//	}
//	EndPeekToken();
//	if(tokens[i-1] == TK_EQUAL){
//		type = 1;
//		if(tokens[i-1] == TK_SEMICOLON){
//			type = 2;
//		}
//	}

	int type = -1;
	StartPeekToken();
	while(current_token.kind != TK_RANGE){
		NO_TOKEN;
		if(current_token.kind == TK_ASSIGN){
			type = 1;
		}else if(current_token.kind == TK_INIT_ASSIGN){
			type = 2;
		}
		NEXT_TOKEN;
	}
	EndPeekToken();

	if(type == 1){
		ParseExpressionList();
		EXPECT(TK_ASSIGN);
	}else if(type == 2){
		ParseIdentifierList();
		EXPECT(TK_INIT_ASSIGN);
	}else{
		// do nothing
	}

	// 跳过range
	expect_token(TK_RANGE);

	// TODO 暂时把range后面的表达式当普通表达式处理。
	ParseRangeExpr();

	AstStatement stmt = NULL;

	return stmt;
}

/**
 * ForStmt = "for" [ Condition | ForClause | RangeClause ] Block .

Condition = Expression .
ForClause = [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] .
InitStmt = SimpleStmt .
PostStmt = SimpleStmt .

RangeClause = [ ExpressionList "=" | IdentifierList ":=" ] "range" Expression .
 */
AstStatement ParseForStmt(){
	// 跳过 for
	NEXT_TOKEN;

	// 处理[ Condition | ForClause | RangeClause ]
	// 识别类型
	StartPeekToken();	
	int type = -1;
	char flag_semicolon = 0;
	// TODO 循环条件能改成 != TK_RBRACE  吗？
	// while(1){
	while(current_token.kind != TK_LBRACE){
		NO_TOKEN;
		if(current_token.kind == TK_RANGE){
			type = 1;
			NEXT_TOKEN;
			break;
		}else if(current_token.kind == TK_SEMICOLON){
			flag_semicolon++;
			NEXT_TOKEN;
			if(flag_semicolon == 2){
				type = 2;
				break;
			}
			// NEXT_TOKEN;
		}else{
			type = 3;
			NEXT_TOKEN;
//			break;
		}
	}

	if(flag_semicolon > 0 && flag_semicolon != 2){
		ERROR("%s\n", "For statement requires two semicolons");
	}

	EndPeekToken();	

	if(type == 1){
		ParseRangeClause();
	}else if(type == 2){
		ParseForClause();
	}else{
		ParseCondition();
	}


	// 处理 Block
	ParseFunctionBody();	

	// TODO 没有建立AST	
	AstStatement stmt;
	CREATE_AST_NODE(stmt, Statement);

	return stmt;
}

/**
 * TypeList        = Type { "," Type } .
 */
AstNode ParseTypeList(){

	ParseType();
	while(current_token.kind == TK_COMMA){
		NEXT_TOKEN;
		ParseType();
	}

	AstStatement stmt;
	return (AstNode)stmt;
}

/**
 * "case" TypeList | "default" .
 */
AstStatement ParseTypeSwitchCase(){

	if(current_token.kind == TK_CASE){
		NEXT_TOKEN;
		ParseTypeList();
	}else if(current_token.kind == TK_DEFAULT){
		NEXT_TOKEN;
	}else{
		ERROR("%s\n", "TypeSwitchCase requires default or case");
	}

	AstStatement stmt;
	return stmt;
}

AstStatement ParseTypeCaseClause(){
	while(current_token.kind == TK_CASE || current_token.kind == TK_DEFAULT){
		ParseTypeSwitchCase();
		expect_token(TK_COLON);
		ParseStatementList();
	}
	
	AstStatement stmt;
	return stmt;
}

/**
 * [ identifier ":=" ] PrimaryExpr "." "(" "type" ")" .
 */
AstStatement ParseTypeSwitchGuard(){

//	StartPeekToken();
//	char colon_flag = 0;
//	char colon_equal_flag = 0;
//	while(current_token.kind != TK_DOT){
//		NO_TOKEN;
//	//	if(current_token.kind == TK_COLON){
//	//		colon_flag = 1;
//	//	}
//		if(colon_flag == 1){
//			if(current_token.kind == TK_EQUAL){
//				colon_equal_flag = 1;
//				break;
//			}else{
//				colon_flag = 0;
//			}
//		}	
//		if(current_token.kind == TK_COLON){
//			colon_flag = 1;
//		}
//		
//		NEXT_TOKEN;
//	}
//	EndPeekToken();
//
//	if(colon_flag == 1){
//		ParseIdentifier();
//		expect_token(TK_COLON);
//		expect_token(TK_EQUAL);
//	}


	StartPeekToken();
	char colon_equal_flag = 0;
	while(current_token.kind != TK_DOT){
		if(current_token.kind == TK_INIT_ASSIGN){
			colon_equal_flag = 1;
		}
		NEXT_TOKEN;
	}
	EndPeekToken();

	if(colon_equal_flag == 1){
		ParseIdentifier();
		EXPECT(TK_INIT_ASSIGN);
	}

	ParsePrimaryExpr();

	// 处理 "." "(" "type" ")"
	expect_token(TK_DOT);
	expect_token(TK_LPARENTHESES);
	expect_token(TK_TYPE);
	expect_token(TK_RPARENTHESES);
	

	AstStatement stmt;
	return stmt;
}

AstStatement ParseExprSwitchCase(){

	if(current_token.kind == TK_DEFAULT){
		// do nothing	
		NEXT_TOKEN;
	}else if(current_token.kind == TK_CASE){
		NEXT_TOKEN;
		ParseExpressionList();
	}else{
		ERROR("%s\n", "ExprSwitchCase requires case or default");
	}

	AstStatement stmt;
	CREATE_AST_NODE(stmt, Statement);
	return stmt;
}

AstStatement ParseExprCaseClause(){
	
	while(current_token.kind == TK_DEFAULT || current_token.kind == TK_CASE){
		ParseExprSwitchCase();
		expect_token(TK_COLON);
		ParseStatementList();
	}

	AstStatement stmt;
	CREATE_AST_NODE(stmt, Statement);

	return stmt;
}

AstStatement ParseTypeSwitchStmt(){

	// 跳过switch
//	NEXT_TOKEN;
	
	// 处理 [ SimpleStmt ";" ]
	// 很容易。在TK_DOT前发现了分号，则存在SimpleStmt。
	// 检查有没有分号。
	char semicolon_flag = 0;
	StartPeekToken();
	while(current_token.kind != TK_DOT){
		NO_TOKEN;
		if(current_token.kind == TK_SEMICOLON){
			semicolon_flag = 1;
			break;
		}
		NEXT_TOKEN;
	}
	EndPeekToken();

	if(semicolon_flag == 1){
		ParseSimpleStatement();
	}

	// 处理 TypeSwitchGuard
	ParseTypeSwitchGuard();

	// 处理 "{" { TypeCaseClause } "}"
	expect_token(TK_LBRACE);
	ParseTypeCaseClause();
	expect_token(TK_RBRACE);
	expect_semicolon;

	AstStatement stmt;

	return stmt;
}

AstStatement ParseExprSwitchStmt(){
	// 跳过switch
//	NEXT_TOKEN;

	// 处理[ SimpleStmt ";" ] [ Expression ]	
	int tokens[200];
	int i = -1;
	char flag = 0;
	StartPeekToken();
	while(current_token.kind != TK_LBRACE){ // {
		NO_TOKEN;
		tokens[++i] = current_token.kind;
		NEXT_TOKEN;
		if(current_token.kind == TK_LBRACE){ // {
			tokens[++i] = current_token.kind;
			flag = 1;
			break;
		}
	}

	if(flag == 0){
		if(current_token.kind != TK_LBRACE){
			ERROR("%s\n", "Require a { in ExprSwitchStmt");
		}else{
			tokens[++i] = current_token.kind;
		}
	}
	
	int j = 0;
	char semicolon_flag = 0;
	for(; j <= i; j++){
		if(tokens[j] == TK_SEMICOLON){
			semicolon_flag = 1;
			break;
		}
	}	

	int type = -1;
	if(semicolon_flag == 1){
		type = 1;
		if(i - j > 1){
			type = 3;
		}
	}else{
		if(i >= 0){
			type = 2;
		}
	}

	EndPeekToken();

	// 处理 [ SimpleStmt ";" ] [ Expression ]
	if(type == 1){
		ParseSimpleStatement();	
		expect_token(TK_SEMICOLON);
	}else if(type == 2){
		ParseExpression();
	}else if(type == 3){
		ParseSimpleStatement();
		expect_token(TK_SEMICOLON);
		ParseExpression();
	}

	// 处理 "{" { ExprCaseClause } "}"
	expect_token(TK_LBRACE);
	ParseExprCaseClause();
	expect_token(TK_RBRACE);
	expect_semicolon;
	
	AstStatement stmt;
	CREATE_AST_NODE(stmt, Statement);
	
	return stmt;
}

AstStatement ParseSwitchStmt(){
	
	expect_token(TK_SWITCH);

	// 识别是ExprSwitchStmt还是TypeSwitchStmt
	char switch_type = -1;
	StartPeekToken();
	char dot_flag = 0;
	while(current_token.kind != TK_LBRACE){
		NO_TOKEN;
		if(current_token.kind == TK_DOT){ // .
			StartPeekToken();
			dot_flag = 1;
			NEXT_TOKEN;
			if(current_token.kind == TK_LPARENTHESES){ // (
				NEXT_TOKEN;
				if(current_token.kind == TK_TYPE){ // type
					NEXT_TOKEN;
					if(current_token.kind == TK_RPARENTHESES){ // )
						switch_type = 2;
						break;
					}
				}else{
					EndPeekToken();
				}
			}else{
				EndPeekToken();
			}
		}
		switch_type = 1;
		NEXT_TOKEN;
	}
	EndPeekToken();

	if(switch_type == 1){
		ParseExprSwitchStmt();	
	}else if(switch_type == 2){
		ParseTypeSwitchStmt();
	}else{
		ERROR("%s\n", "Require a switch stmt");
	}

	AstStatement stmt;
	CREATE_AST_NODE(stmt, Statement);

	return stmt;
}

/**
 * BreakStmt = "break" [ Label ] .
 * Label       = identifier .
 */
AstStatement ParseBreakStmt(){
	EXPECT(TK_BREAK);
	if(current_token.kind == TK_ID){
		ParseIdentifier();
	}

	AstStatement stmt;
	CREATE_AST_NODE(stmt, Statement);

	return stmt;
}

