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
		case TK_FUNC:
			LOG("%s\n", "parse func");
			ParseFunctionDecl();
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

	AstDeclaration head;
	CREATE_AST_NODE(head, Declaration);
	head->next = NULL;
	AstDeclaration preDeclaration = NULL;
	AstDeclaration curDeclaration;
	AstDeclaration declaration;
//	AstDeclaration;

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
		curDeclaration = ParseConstSpec();
		if(head->next == NULL){
			head->next = curDeclaration;
		}

		if(preDeclaration == NULL){
			preDeclaration = curDeclaration;
		}else{
			preDeclaration->next = curDeclaration;
			preDeclaration = curDeclaration;
		}
		// curDeclaration = ParseConstSpec();
		expect_semicolon;
		// if(current_token.kind == TK_SEMICOLON) expect_token(TK_SEMICOLON);
		//expect_token(TK_SEMICOLON);
	}
	curDeclaration->next = NULL;
	expect_token(TK_RPARENTHESES);
	declaration = (AstDeclaration)head->next;
}else{
	declaration = ParseConstSpec();
}
	return declaration;
}

/**
 * ConstSpec      = IdentifierList [ [ Type ] "=" ExpressionList ] .
 */
AstDeclaration ParseConstSpec(){
	LOG("%s\n", "parse const spec");
//	ParseIdentifierList();
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression); 
	expr = ParseExpressionList();

	AstExpression expr2;
	CREATE_AST_NODE(expr2, Expression); 

	AstNode type;
	CREATE_AST_NODE(type, Node); 
//	if(current_token.kind == TK_TYPE){
	if(IsDataType(current_token.value.value_str) == 1){
//		NEXT_TOKEN;
		type = ParseType();
		assert(current_token.kind == TK_ASSIGN);
		expect_token(TK_ASSIGN);
		expr2 = ParseExpressionList();
	}else if(current_token.kind == TK_ASSIGN){
		NEXT_TOKEN;
		expr2 = ParseExpressionList();
	}else{

		NEXT_TOKEN;
	}


	// 遍历expr和expr2开头的单链表，创建新的单链表。
	AstDeclaration declaration;
	CREATE_AST_NODE(declaration, Declaration);
	
	// 是一个单链表A的第一个结点。
	AstInitDeclarator initDecs;
	CREATE_AST_NODE(initDecs, InitDeclarator);

	AstSpecifiers specs;
	CREATE_AST_NODE(specs, Specifiers);
	specs->tySpecs = type;

	// 遍历单链表B和C，创建A。
	// todo 照抄ParseVarSpec，寻机优化。
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
		strcpy(dec->id, exprCur->val.p);
		initDecsCur->dec = dec;

		AstInitializer init;
		CREATE_AST_NODE(init, Initializer);
		AstExpression expr;
		CREATE_AST_NODE(expr, Expression);
		expr->val.i[0] = expr2Cur->val.i[0];
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

// IdentifierList = identifier { "," identifier }
AstDeclarator ParseIdentifierList(){
	LOG("%s\n", "parse IdentifierList");

	int count = 0;
	AstDeclarator decl = ParseIdentifier();
	if(decl == NULL){
		CREATE_AST_NODE(decl, NameDeclarator);
		decl->variable_count = 0;
		return decl;
	}
	count++;
	AstDeclarator *tail;
	tail = &(decl->next);

	// ParseIdentifier();
	while(current_token.kind == TK_COMMA){
		NEXT_TOKEN;
		*tail = ParseIdentifier();
		if(*tail == NULL){
			break;
		}
		count++;
		tail = &((*tail)->next);
		
		StartPeekToken();
		NEXT_TOKEN;
		if(current_token.kind != TK_COMMA){
			EndPeekToken();
			*tail = NULL;
			break;
		}
	}

	decl->variable_count = count;

	return decl;
}

AstDeclarator ParseIdentifier(){
	LOG("%s\n", "parse Identifier");
	AstDeclarator decl = NULL;
//	CREATE_AST_NODE(decl, NameDeclarator);
	// todo 不知道有没有问题。
	// 我想用这种方式处理[Identifier]产生式。
	if(current_token.kind == TK_ID){
//		NEXT_TOKEN;
		CREATE_AST_NODE(decl, NameDeclarator);
		decl->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
		strcpy(decl->id, current_token.value.value_str);
		NEXT_TOKEN;
	}

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

/**
 * TypeDecl = "type" ( TypeSpec | "(" { TypeSpec ";" } ")" ) .
TypeSpec = AliasDecl | TypeDef .

把这两种不同的产生式写在了一个函数中。
AliasDecl = identifier "=" Type .
TypeDef = identifier Type .
 */
AstDeclaration ParseTypeSpec(){
	LOG("%s\n", "parse TypeSpec");
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression); 
	// expr = ParseExpressionList();
	expr = ParseExpression();

	AstNode type;
	CREATE_AST_NODE(type, Node); 

	if(current_token.kind == TK_ASSIGN){
		NEXT_TOKEN;
		// expr = ParseExpressionList();
		expr = ParseExpression();
	}else{
		type = ParseType();
	}

	AstDeclaration declaration;
	CREATE_AST_NODE(declaration, Declaration);
	
	AstInitDeclarator initDecs;
	CREATE_AST_NODE(initDecs, InitDeclarator);

	AstDeclarator dec;
	CREATE_AST_NODE(dec, Declarator);
	dec->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
	strcpy(dec->id, expr->val.p);
	initDecs->dec = dec;

	AstSpecifiers specs;
	CREATE_AST_NODE(specs, Specifiers);
	specs->tySpecs = type;

	declaration->specs = specs; 
	declaration->initDecs = initDecs; 

	return declaration;
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

AstNode ParseTypeDecl(){
//	expect_token(TK_TYPE);
//	expect_token(TK_ID);
//	ParseType();

	AstDeclaration declaration;
	CREATE_AST_NODE(declaration, Declaration);

	AstDeclaration curDeclaration;// = declaration;
	CREATE_AST_NODE(curDeclaration, Declaration);
	AstDeclaration preDeclaration;// = declaration;
//	CREATE_AST_NODE(preDeclaration, Declaration);
	preDeclaration = NULL;

	LOG("%s\n", "parse TypeDec");
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		AstDeclaration headDeclaration;// = declaration;
		CREATE_AST_NODE(headDeclaration, Declaration);
		headDeclaration->next = NULL;
		NEXT_TOKEN;
		while(current_token.kind != TK_RPARENTHESES){
			// CREATE_AST_NODE(curDeclaration, Declaration);
			// todo 耗费了很多很多时间才处理好。
			curDeclaration = ParseTypeSpec();
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
		declaration = ParseTypeSpec();
	}

	return declaration;
}


/**
 * ParameterDecl  = [ IdentifierList ] [ "..." ] Type .
 */
AstParameterDeclaration ParseParameterDecl(int *count){
	AstParameterDeclaration decl;
	// todo 这一语句是必需的吗？
	CREATE_AST_NODE(decl, ParameterDeclaration);
	AstDeclarator dec;
	CREATE_AST_NODE(dec, Declarator);
	decl->dec = dec;

	int expr_count = 0;
	// todo 等会再修改expr
	AstDeclarator expr;
	// TK_ELLIPSIS 是省略号，即...。
	// 当前token不是省略号，也不是数据类型，把当前token当表达式解析。
	// todo 能解析出一个表达式也不存在吗？
	if(current_token.kind != TK_ELLIPSIS && IsDataType(current_token.value.value_str) == 0){
		expr = ParseIdentifierList();
		expr_count = expr->variable_count;
	} 
	expect_ellipsis;
	// todo 暂时只支持int等数据类型。
	AstTypedefName type = ParseType();
	AstSpecifiers specs;
        CREATE_AST_NODE(specs, Specifiers);
        specs->tySpecs = type;

	if(expr_count == 1){
		decl->specs = specs;
		decl->dec->id = (char *)malloc(sizeof(char)*MAX_NAME_LEN);
		strcpy(decl->dec->id, (char *)(expr->id));
	}else if(expr_count > 1){
		AstParameterDeclaration *tail = &decl;
//		AstExpression cur = expr;
		AstDeclarator cur = expr;
//		AstDeclarator decl;
//		CREATE_AST_NODE(decl, NameDeclarator);
//		AstDeclarator dec;
//		CREATE_AST_NODE(dec, NameDeclarator);
		// while(cur != NULL){
//		while(cur != 345){
////			(*tail)->specs = type;
//			AstDeclarator dec;
//			CREATE_AST_NODE(dec, NameDeclarator);
//			dec->id = (char *)malloc(sizeof(char)*MAX_NAME_LEN);
//			strcpy(dec->id, (char *)(cur->id));
//			(*tail)->dec = cur;
//		//	(*tail)->dec->id = (char *)malloc(sizeof(char)*MAX_NAME_LEN);
//			// strcpy((*tail)->dec->id, (char *)(cur->val.p));
//			// strcpy(dec->id, (char *)(cur->val.p));
//			tail = &((*tail)->next);
//			cur = cur->next;
//		}

		AstParameterDeclaration pre;
		AstParameterDeclaration curDecl;
		AstParameterDeclaration headDecl;
		CREATE_AST_NODE(headDecl, ParameterDeclaration);
		headDecl->next = NULL;
		pre = NULL;
		curDecl = decl;
		while(cur != NULL){
			if(headDecl->next == NULL){
				headDecl->next = decl;
			}	
			
			if(pre == NULL){
				pre = decl;
			}else{
				// pre = curDecl;
				CREATE_AST_NODE(curDecl, ParameterDeclaration);
				pre->next = curDecl;
				pre = curDecl;
			}
			curDecl->dec = cur;
			curDecl->specs = specs;
			cur = cur->next;
		}
		pre->next = NULL;
		decl = headDecl->next;
	}else{
		// todo 想不明白。暂时不处理。
		// max(int, int, int)
		decl->specs = specs;
		decl->dec->id = 0x0;
	}

	*count = expr_count;

	return decl;
}

// ParameterList  = ParameterDecl { "," ParameterDecl } .
AstParameterDeclaration ParseParameterList(){
	
	AstParameterDeclaration decl;
	int *count = (int *)malloc(sizeof(int));;
	*count = 0;
	decl = ParseParameterDecl(count);
	AstParameterDeclaration *tail = &(decl->next);

	// todo 当count是0时，也使用下面的代码处理吗？
	if(*count <= 1){
		// AstParameterDeclaration *tail = &(decl->next);
		int *count_useless = (int *)malloc(sizeof(int));
		*count_useless = 0;
		while(current_token.kind == TK_COMMA){
			NEXT_TOKEN;
			*tail = ParseParameterDecl(count_useless);		
			tail = &((*tail)->next);
		}
		*tail = NULL;
	}else{
		// todo 这种情况下，似乎不用进行任何处理。
//		AstParameterDeclaration current = decl->next;
//		while(current != NULL){
//			*tail = current;	
//			current = current->next;
//		}
	}

	return decl;
}

/**
 * Parameters     = "(" [ ParameterList [ "," ] ] ")" .
 */
AstParameterDeclaration ParseParameters(){
	expect_token(TK_LPARENTHESES);
	AstParameterDeclaration parameterList = ParseParameterList();
	expect_comma;
//	while(current_token.kind != TK_RPARENTHESES){
//
//	}
	expect_token(TK_RPARENTHESES);

	return parameterList;
}

// Result         = Parameters | Type .
// 很奇特的产生式。只要Parameters就能表示所有的返回值情况，为啥要一个Type呢？
// 为了处理只有一个返回值的情况。能处理没有返回值的情况吗？
// 这取决于Type是否可能为空值。
AstParameterDeclaration ParseResult(){
	AstParameterDeclaration parameterList;
	if(current_token.kind == TK_LPARENTHESES){
		parameterList = ParseParameters();
	}else if(current_token.kind == TK_LBRACE){
		// 当参数后的token是{时，说明没有返回值。
		// todo 可在没有函数体时，这样又不正确。
		// TK_LBRACE 是 {
//		CREATE_AST_NODE(parameterList, ParameterDeclaration);
//		// todo 没有返回值时如何处理？暂时没有实现。	
//		parameterList->specs = ParseType();
		parameterList = NULL;
	}else{
		CREATE_AST_NODE(parameterList, ParameterDeclaration);
		// todo 没有返回值时如何处理？暂时没有实现。	
		parameterList->specs = ParseType();
		parameterList->dec = NULL;
	}	

	return parameterList;
}

AstNode ParseFunctionName(){

	// todo 怎么确保这个位置的token是TK_ID？
	// AstExpression identifier = ParseExpression();	
	// AstExpression identifier = ParsePrimaryExpr();	
	// todo 直接使用ParseOperandName能避免使用IsPostfix，但是，合适吗？
	AstExpression identifier = ParseOperandName();
	AstDeclarator functionName;
	CREATE_AST_NODE(functionName, Declarator);
	functionName->id = (char *)malloc(sizeof(char));
	strcpy(functionName->id, (char *)identifier->val.p);

	return functionName;
}

// todo 不要。
AstNode ParseSignature(){

}

AstNode ParseFunctionBody(){

}


void PrintFdec(AstFunctionDeclarator fdec){
	AstParameterTypeList paramTypeList = fdec->paramTyList;
	AstParameterDeclaration pdec = (AstParameterDeclaration)fdec->paramTyList->paramDecls;	
	int count = 0;
	while(pdec != NULL){
		printf("data-type%d = %s\n", count, ((AstTypedefName)pdec->specs->tySpecs)->id);
		printf("param%d = %s\n", count, ((AstParameterDeclaration)(pdec))->dec->id);
		pdec = (AstParameterDeclaration)pdec->next;
		count++;
	}

	AstParameterTypeList sig = fdec->sig;
	AstParameterDeclaration rdec = (AstParameterDeclaration)sig->paramDecls;	
	int rcount = 0;
	while(rdec != NULL){
		printf("pdata-type%d = %s\n", rcount, ((AstTypedefName)rdec->specs->tySpecs)->id);
		printf("return%d = %s\n", rcount, ((AstParameterDeclaration)(rdec))->dec->id);
		rdec = (AstParameterDeclaration)rdec->next;
		rcount++;
	}
}

/**
 * FunctionDecl = "func" FunctionName Signature [ FunctionBody ] .
 */
AstNode ParseFunctionDecl(){

	NEXT_TOKEN;

	AstFunctionDeclarator fdec;
	CREATE_AST_NODE(fdec, FunctionDeclarator);

	AstNode functionName = ParseFunctionName();
	fdec->dec = functionName;

	AstParameterTypeList paramTypeList;
	CREATE_AST_NODE(paramTypeList, ParameterTypeList);
	AstParameterDeclaration params = ParseParameters(); 
	paramTypeList->paramDecls = params;
	fdec->paramTyList = paramTypeList;
	// todo 测试，打印数据。
//	PrintFdec(fdec);
//	printf("data-type = %s\n", ((AstTypedefName)fdec->paramTyList->paramDecls->specs->tySpecs)->id);
//	printf("param2 = %s\n", ((AstParameterDeclaration)(fdec->paramTyList->paramDecls->next))->dec->id);
	
	AstParameterTypeList signature;
	CREATE_AST_NODE(signature, ParameterTypeList);
	AstParameterDeclaration result = ParseResult(); 
	signature->paramDecls = result;
	fdec->sig = signature;
//	printf("return data-type = %s\n", ((AstTypedefName)fdec->sig->paramDecls->specs->tySpecs)->id);
//	printf("return2 = %s\n", ((AstParameterDeclaration)(fdec->sig->paramDecls->next))->dec->id);


	
	// todo 测试，打印数据。
	PrintFdec(fdec);
	
		
	AstFunction func;
	CREATE_AST_NODE(func, Function);
	
	func->fdec = fdec;

	// todo 暂时不处理FunctionBody
	
	return (AstNode)func;
}
