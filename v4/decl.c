#include "ast.h"
#include "decl.h"
#include "expr.h"


AstNode declaration(){
	LOG("%s\n", "enter declaration");	
	TokenKind kind = current_token.kind;
	Value value = current_token.value;

	AstNode decl;

	switch(kind){
		case TK_VAR:
			LOG("%s\n", "parse var");
			ParseVarDecl();
			break;
		case TK_TYPE:
			LOG("%s\n", "parse type");
			ParseTypeDecl();
			break;
		case TK_CONST:
			LOG("%s\n", "parse const");
			ParseConstDecl();
			break;
		default:
			LOG("%s\n", "parse decl error");
			break;
	}

	return decl;
}

// todo 寻机把这个宏放到更合适的位置。
// #define expect_semicolon if(current_token.kind == TK_SEMICOLON) expect_token(TK_SEMICOLON);
/**
 * 	ConstDecl      = "const" ( ConstSpec | "(" { ConstSpec ";" } ")" ) .
	ConstSpec      = IdentifierList [ [ Type ] "=" ExpressionList ] .
 */
AstNode ParseConstDecl(){
	LOG("%s\n", "parse const decl");

	AstNode decl;
	expect_token(TK_CONST);
	if(current_token.kind == TK_LPARENTHESES){
	NEXT_TOKEN;	
//	ParseConstSpec();
//	expect_semicolon;
	// if(current_token.kind == TK_SEMICOLON) expect_token(TK_SEMICOLON);
	// todo 耗费了很多很多时间。
	// while(current_token.kind == TK_SEMICOLON){
	while(current_token.kind == TK_ID){
		// todo 不需要这个NEXT_TOKEN，留给下面的函数解析。花了很多很多时间才找出这个问题。
		//NEXT_TOKEN;	
		ParseConstSpec();
		expect_semicolon;
		// if(current_token.kind == TK_SEMICOLON) expect_token(TK_SEMICOLON);
		//expect_token(TK_SEMICOLON);
	}
	expect_token(TK_RPARENTHESES);
}else{
	ParseConstSpec();
}

	return decl;
}

AstNode ParseConstSpec(){
	LOG("%s\n", "parse const spec");

	ParseIdentifierList();
//	if(current_token.kind == TK_TYPE){
	if(IsDataType(current_token.value.value_str) == 1){
		NEXT_TOKEN;
		assert(current_token.kind == TK_ASSIGN);
		expect_token(TK_ASSIGN);
		ParseExpressionList();
	}else if(current_token.kind == TK_ASSIGN){
		NEXT_TOKEN;
		ParseExpressionList();
	}else{

		NEXT_TOKEN;
	}

	// todo 
	AstNode decl;
	return decl;
}

// IdentifierList = identifier { "," identifier }
AstNode ParseIdentifierList(){
	LOG("%s\n", "parse IdentifierList");

	ParseIdentifier();
	while(current_token.kind == TK_COMMA ){
		NEXT_TOKEN;
		ParseIdentifier();
	}

	AstNode decl;

	return decl;
}

AstNode ParseIdentifier(){
	LOG("%s\n", "parse Identifier");
	// todo 不知道有没有问题。
	// 我想用这种方式处理[Identifier]产生式。
	if(current_token.kind == TK_ID){
		NEXT_TOKEN;
	}

	AstNode decl;
	return decl;
}

void ExpectDataType(){
	if(IsDataType(current_token.value.value_str) == 1){
		NEXT_TOKEN;
	}else{
		ERROR("%s\n", "expect a data type");
		exit(-1);	
	}
}

int IsDataType(char *str){
	int count = sizeof(TypeNames) / sizeof(TypeNames[0]);
	for(int i = 0; i < count; i++){
		if(strcmp(str, TypeNames[i]) == 0){
			return 1;	
		}
	}

	return 0;
}

/**
 * VarDecl     = "var" ( VarSpec | "(" { VarSpec ";" } ")" ) .
VarSpec     = IdentifierList ( Type [ "=" ExpressionList ] | "=" ExpressionList ) .
 */
AstNode ParseVarDecl(){

	AstDeclaration declaration;
	CREATE_AST_NODE(declaration, Declaration);

	AstDeclaration curDeclaration;// = declaration;
	CREATE_AST_NODE(curDeclaration, Declaration);
	AstDeclaration preDeclaration;// = declaration;
//	CREATE_AST_NODE(preDeclaration, Declaration);
	preDeclaration = NULL;

	LOG("%s\n", "parse VarDec");
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		AstDeclaration headDeclaration;// = declaration;
		CREATE_AST_NODE(headDeclaration, Declaration);
		headDeclaration->next = NULL;
		NEXT_TOKEN;
		while(current_token.kind != TK_RPARENTHESES){
			// CREATE_AST_NODE(curDeclaration, Declaration);
			// todo 耗费了很多很多时间才处理好。
			curDeclaration = ParseVarSpec();
			if(headDeclaration->next == NULL){
				headDeclaration->next = (AstNode)curDeclaration;
			}
//			CREATE_AST_NODE(preDeclaration, Declaration);
			if(preDeclaration == NULL){
				preDeclaration = curDeclaration;
			}else{
				preDeclaration->next = (AstNode)curDeclaration;
				preDeclaration = curDeclaration;
			}
// 			preDeclaration->next = (AstNode)curDeclaration;
// 			preDeclaration = curDeclaration;
			CREATE_AST_NODE(curDeclaration, Declaration);
			// preDeclaration->next = (AstNode)curDeclaration;
			// CREATE_AST_NODE(curDeclaration, Declaration);
			// preDeclaration = curDeclaration;
			// preDeclaration->next = curDeclaration;
			expect_semicolon;	
		}
		curDeclaration->next = NULL;
		expect_token(TK_RPARENTHESES);
		declaration = (AstDeclaration)headDeclaration->next;
	}else{
//		CREATE_AST_NODE(curDeclaration, Declaration);
//		curDeclaration = ParseVarSpec();
		declaration = ParseVarSpec();
	}

	return declaration;
}

/**
 * VarSpec     = IdentifierList ( Type [ "=" ExpressionList ] | "=" ExpressionList ) .
 */
AstDeclaration ParseVarSpec(){
	LOG("%s\n", "parse VarDec");
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression); 
	AstExpression expr2;
	CREATE_AST_NODE(expr2, Expression); 
//	ParseIdentifierList();
	expr2 = ParseExpressionList();

	AstNode type;
	CREATE_AST_NODE(type, Node); 

	if(current_token.kind == TK_ASSIGN){
		NEXT_TOKEN;
		expr = ParseExpressionList();
	}else{
		// 跳过Type
		// NEXT_TOKEN;
	//	AstNode type;
	//	CREATE_AST_NODE(type, Node); 
		type = ParseType();
		if(current_token.kind == TK_ASSIGN){
			NEXT_TOKEN;
			expr = ParseExpressionList();
		}
	}

	AstDeclaration declaration;
	CREATE_AST_NODE(declaration, Declaration);
	
	// 是一个单链表A的第一个结点。
	AstInitDeclarator initDecs;
	CREATE_AST_NODE(initDecs, InitDeclarator);

	AstSpecifiers specs;
	CREATE_AST_NODE(specs, Specifiers);
	specs->tySpecs = type;

	// 遍历单链表B和C，创建A。
	AstInitDeclarator preInitDecs = initDecs;
	AstInitDeclarator initDecsCur = initDecs;

	AstExpression exprCur = expr;
	AstExpression expr2Cur = expr2;
	while(exprCur != NULL){
		// initDecsCur->dec->id = exprCur->val;	
		// initDecsCur->dec->id = exprCur->val;	
		AstDeclarator dec;
		CREATE_AST_NODE(dec, Declarator);
		dec->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
		strcpy(dec->id, expr2Cur->val.p);
		initDecsCur->dec = dec;

		AstInitializer init;
		CREATE_AST_NODE(init, Initializer);
		AstExpression expr;
		CREATE_AST_NODE(expr, Expression);
		expr->val.i[0] = exprCur->val.i[0];
		init->expr = expr;
		initDecsCur->init = init;

		preInitDecs = initDecsCur;
		CREATE_AST_NODE(initDecsCur, InitDeclarator);
		preInitDecs->next = initDecsCur;

		exprCur = exprCur->next;
		expr2Cur = expr2Cur->next;
	}

	declaration->specs = specs; 
	declaration->initDecs = initDecs; 

	return declaration;
}

AstNode ParseTypeDecl(){
	expect_token(TK_TYPE);
	expect_token(TK_ID);
	ParseType();
}

/**
 * FieldDecl     = (IdentifierList Type | EmbeddedField) [ Tag ] .
 */
AstNode ParseFieldDecl(){
	if(current_token.kind == TK_MUL){
		ParseEmbeddedField();
	}else{
		ParseIdentifierList();
		ParseType();
	}
//	ParseTag();
}

/**
 * EmbeddedField = [ "*" ] TypeName .
 */
AstNode ParseEmbeddedField(){
	expect_token(TK_MUL);
	ParseTypeName();
}

/**
 * Tag           = string_lit .
 */
// todo 先用简单方式处理
// todo 不知Tag在struct有什么用。它导致错误，先注释它。
AstNode ParseTag(){
//	expect_token(TK_TYPE);
//	expect_token(TK_ID);
//	ParseType();
	ParseStrintLit();
}

/**
 * string_lit             = raw_string_lit | interpreted_string_lit .
raw_string_lit         = "`" { unicode_char | newline } "`" .
interpreted_string_lit = `"` { unicode_value | byte_value } `"` .
 */
// todo 暂时用简单方式处理。
AstNode ParseStrintLit(){
	NEXT_TOKEN;

}

/**
 * ParameterDecl  = [ IdentifierList ] [ "..." ] Type .
 */
AstNode ParseParameterDecl(){
	ParseIdentifierList();
	expect_ellipsis;
	ParseType();
}

//AstNode ParseVarDecl(){
//	LOG("%s\n", "parse VarDec");
//
//}
//
//
//AstNode ParseVarDecl(){
//	LOG("%s\n", "parse VarDec");
//
//}
//
//
//AstNode ParseVarDecl(){
//	LOG("%s\n", "parse VarDec");
//
//}
//
//
//AstNode ParseVarDecl(){
//	LOG("%s\n", "parse VarDec");
//
//}
