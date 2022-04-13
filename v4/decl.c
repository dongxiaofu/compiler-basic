#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
// #include "stmt.h"


AstNode declaration(){
	LOG("%s\n", "enter declaration");	
	TokenKind kind = current_token.kind;
	Value value = current_token.value;

	AstNode decl;
	CREATE_AST_NODE(decl, Node);

	switch(kind){
		case TK_VAR:
			LOG("%s\n", "parse var");
			decl = (AstNode)ParseVarDecl();
			break;
		case TK_TYPE:
			LOG("%s\n", "parse type");
			decl = (AstNode)ParseTypeDecl();
			break;
		case TK_CONST:
			LOG("%s\n", "parse const");
			decl = (AstNode)ParseConstDecl();
			break;
		case TK_FUNC:
			LOG("%s\n", "parse func");
			decl = ParseMethodDeclOrFunctionDecl();
			break;
		default:
			LOG("%s\n", "parse decl error");
			printf("parse decl error\n");
			exit(-3);
			break;
	}

	return decl;
}

/**
 * 	ConstDecl      = "const" ( ConstSpec | "(" { ConstSpec ";" } ")" ) .
	ConstSpec      = IdentifierList [ [ Type ] "=" ExpressionList ] .
 */
// AstNode ParseConstDecl(){
AstConstDeclaration ParseConstDecl(){
	LOG("%s\n", "parse const decl");

	AstConstDeclarator head;
	CREATE_AST_NODE(head, ConstDeclarator);

	AstConstDeclarator preDeclaration = NULL;
	AstConstDeclarator  curDeclaration;

	AstConstDeclaration declaration;

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
	
			expect_semicolon;
		}
		curDeclaration->next = NULL;
		expect_token(TK_RPARENTHESES);
		expect_token(TK_SEMICOLON);
		declaration->decs = (AstConstDeclarator)head->next;
	}else{
		declaration->decs = ParseConstSpec();
		expect_semicolon;
	}

	return declaration;
}

/**
 * ConstSpec      = IdentifierList [ [ Type ] "=" ExpressionList ] .
 */
// AstDeclaration ParseConstSpec(){
AstConstDeclarator ParseConstSpec(){
	LOG("%s\n", "parse const spec");
	AstExpression expr = ParseIdentifierList();

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

	//	NEXT_TOKEN;
	}


	// 遍历expr和expr2开头的单链表，创建新的单链表。
	AstConstDeclarator declaration;
	CREATE_AST_NODE(declaration, ConstDeclarator);
	
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

	// TODO 不应该这样命名，只是为了兼容之前的错误写法。
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
		AstExpression expr1;
		CREATE_AST_NODE(expr1, Expression);
		expr1->val.i[0] = expr2Cur->val.i[0];
		init->expr = expr1;
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
AstExpression ParseIdentifierList(){
	LOG("%s\n", "parse IdentifierList");

	int count = 0;
//	AstDeclarator decl = ParseIdentifier();
	AstExpression expr = ParseIdentifier();
	if(expr == NULL){
		CREATE_AST_NODE(expr, Expression);
		expr->variable_count = 0;
		return expr;
	}
	count++;
	AstExpression *tail;
	tail = &(expr->next);

	// ParseIdentifier();
	while(current_token.kind == TK_COMMA){
		NEXT_TOKEN;
		*tail = ParseIdentifier();
		if(*tail == NULL){
			break;
		}
		count++;
		tail = &((*tail)->next);
	}

	expr->variable_count = count;
//	decl->next = NULL;		// 不能少了这一句。
//	(*tail)->next = NULL;

	return expr;
}

AstExpression ParseIdentifier(){
	LOG("%s\n", "parse Identifier");
	AstExpression expr = NULL;
	CREATE_AST_NODE(expr, Expression);
	// expr->val->p = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
	expr->val.p = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
	// TODO 初始化申请到的变量，清除脏数据。
	memset(expr->val.p, 0, sizeof(char) * MAX_NAME_LEN);
	// 我想用这种方式处理[Identifier]产生式。
	if(current_token.kind == TK_ID){
		strcpy(expr->val.p, current_token.value.value_str);
	}else if(current_token.kind == TK_UNDERSCORE){
		strcpy(expr->val.p, "_");
	}else{
		return expr;
	}

	NEXT_TOKEN;

	return expr;
}

// AstDeclarator ParseIdentifier(){
// 	LOG("%s\n", "parse Identifier");
// 	AstDeclarator decl = NULL;
// //	CREATE_AST_NODE(decl, NameDeclarator);
// 	// todo 不知道有没有问题。
// 	// 我想用这种方式处理[Identifier]产生式。
// 	if(current_token.kind == TK_ID){
// //		NEXT_TOKEN;
// 		CREATE_AST_NODE(decl, NameDeclarator);
// 		decl->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
// 		strcpy(decl->id, current_token.value.value_str);
// 		NEXT_TOKEN;
// 	}if(current_token.kind == TK_UNDERSCORE){
// 		// TODO 不一定正确。为了程序能正常运行，只能这样做。
// 		CREATE_AST_NODE(decl, NameDeclarator);
// 		decl->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
// 		strcpy(decl->id, "_");
// 		NEXT_TOKEN;
// 	}
// 
// 	return decl;
// }

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
// TODO 最好找机会优化成变量专用声明。否则，怎么识别这个声明的种类。
// AstNode ParseVarDecl(){
// AstDeclaration ParseVarDecl(){
AstVarDeclaration ParseVarDecl(){

	AstVarDeclaration declaration;
	CREATE_AST_NODE(declaration, VarDeclaration);

	// TODO 命名需要修改。
	AstVarDeclarator curDeclaration;
	CREATE_AST_NODE(curDeclaration, VarDeclarator);
	AstVarDeclarator preDeclaration = NULL;

	LOG("%s\n", "parse VarDec");
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		AstVarDeclarator headDeclaration;
		CREATE_AST_NODE(headDeclaration, VarDeclarator);
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
			expect_semicolon;	
		}
		curDeclaration->next = NULL;
		expect_token(TK_RPARENTHESES);
		expect_token(TK_SEMICOLON);
		declaration->decs = (AstVarDeclarator)headDeclaration->next;
	}else{
//		CREATE_AST_NODE(curDeclaration, Declaration);
//		curDeclaration = ParseVarSpec();
		declaration->decs = ParseVarSpec();
		// TODO 不能确定所有的声明后面都加上了分号。
		expect_semicolon;	
	}

	return declaration;
}

/**
 * VarSpec     = IdentifierList ( Type [ "=" ExpressionList ] | "=" ExpressionList ) .
 */
// AstDeclaration ParseVarSpec(){
AstVarDeclarator ParseVarSpec(){
	LOG("%s\n", "parse VarDec");
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression); 
	AstExpression expr2;
	CREATE_AST_NODE(expr2, Expression); 
	AstExpression decl = ParseIdentifierList();
	// expr2 = ParseExpressionList();

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

	AstVarDeclarator declaration;
	CREATE_AST_NODE(declaration, VarDeclarator);
	
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
	AstExpression expr2Cur = decl;
	// AstDeclaration expr2Cur = decl;
	// AstDeclarator expr2Cur = decl;
	while(exprCur != NULL){
		NO_TOKEN;
		// initDecsCur->dec->id = exprCur->val;	
		// initDecsCur->dec->id = exprCur->val;	
		AstDeclarator dec;
		CREATE_AST_NODE(dec, Declarator);
		dec->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
		strcpy(dec->id, expr2Cur->val.p);
		// strcpy(dec->id, expr2Cur->id);
		initDecsCur->dec = dec;

		AstInitializer init;
		CREATE_AST_NODE(init, Initializer);
		AstExpression expr;
		CREATE_AST_NODE(expr, Expression);
		memcpy(expr, exprCur, sizeof(*expr));
//		if(expr->op == OP_STR){
//			expr->val.i[0] = exprCur->val.i[0];
//		}
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
// AstDeclaration ParseTypeSpec(){
AstTypeDeclarator ParseTypeSpec(){
	LOG("%s\n", "parse TypeSpec");
//	AstExpression expr;
//	CREATE_AST_NODE(expr, Expression); 
//	// expr = ParseExpressionList();
//	expr = ParseExpression();

	AstExpression expr = ParseIdentifier();

	AstNode type;
	CREATE_AST_NODE(type, Node); 

	if(current_token.kind == TK_ASSIGN){
		NEXT_TOKEN;
		// expr = ParseExpressionList();
//		expr = ParseExpression();
	}else{
//		type = ParseType();
	}
	type = ParseType();

	AstTypeDeclarator decl;
	CREATE_AST_NODE(decl, TypeDeclarator);
	
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

	decl->specs = specs; 
	decl->initDecs = initDecs; 

	return dec;
}

/**
 * FieldDecl     = (IdentifierList Type | EmbeddedField) [ Tag ] .
 */
FieldDecl ParseFieldDecl(){
	// TODO 能把局部变量当作函数的返回值吗？
	// FieldDecl decl = (FieldDecl)malloc(sizeof(*FieldDecl));
	FieldDecl decl = (FieldDecl)malloc(sizeof(struct fieldDecl));

	// TODO 怎么存储EmbeddedField？我特别烦这种不规则的语言结构。
	if(current_token.kind == TK_MUL){
		AstPointerDeclarator ptrDecl;
		CREATE_AST_NODE(ptrDecl, PointerDeclarator);

		expect_token(TK_MUL);
		// ParseTypeName 的返回值有两种情况，例如：Sin；Math.Sin。
		// TODO 非常厌恶这种返回值。我期望一个函数能返回结构相同的数据。
		AstNode node = ParseTypeName();
		ptrDecl->dec = (AstDeclarator)node;
		decl->member = ptrDecl;
		decl->tail = ptrDecl;
	}else{
		AstStructDeclarator member = NULL;
		AstStructDeclarator tail = NULL;
		AstExpression expr = ParseIdentifierList();
		AstNode dataType = ParseType();
		AstStructDeclarator cur = NULL;
		int variable_count = expr->variable_count;
		for(int i = 0; i < variable_count; i++){
			if(cur == NULL){
				AstStructDeclarator cur;
				CREATE_AST_NODE(cur, StructDeclarator);
				member = cur;
			}else{
				AstStructDeclarator next;
				CREATE_AST_NODE(next, StructDeclarator);
				cur->next = next;
				cur = next;
			}
			cur->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
			strcpy(cur->id, expr->val.p);
			cur->type = dataType;
		}	

		tail = cur;
		decl->member = member;
		decl->tail = tail;
	}
//	ParseTag();

	return decl;
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
 * TypeDecl = "type" ( TypeSpec | "(" { TypeSpec ";" } ")" ) .
TypeSpec = AliasDecl | TypeDef .
 */
// AstNode ParseTypeDecl(){
AstTypeDeclaration ParseTypeDecl(){

	AstTypeDeclaration declaration;
	CREATE_AST_NODE(declaration, TypeDeclaration);

	// TODO curDeclaration改成curDeclarator会更好，但我不想现在做这些复制粘贴的工作。
	AstTypeDeclarator curDeclaration;
	CREATE_AST_NODE(curDeclaration, TypeDeclarator);
	AstTypeDeclarator preDeclaration = NULL;

	LOG("%s\n", "parse TypeDec");
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		AstTypeDeclarator headDeclaration;
		CREATE_AST_NODE(headDeclaration, TypeDeclarator);
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
		expect_token(TK_SEMICOLON);
		declaration->decs = (AstTypeDeclarator)headDeclaration->next;
	}else{
		declaration->decs = ParseTypeSpec();
		expect_semicolon;
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
	int *count_useless = (int *)malloc(sizeof(int));
	*count_useless = 0;
	while(current_token.kind == TK_COMMA){
		NEXT_TOKEN;
		*tail = ParseParameterDecl(count_useless);		
		tail = &((*tail)->next);
	}
	*tail = NULL;
	return decl;
}

/**
 * Parameters     = "(" [ ParameterList [ "," ] ] ")" .
 */
AstParameterDeclaration ParseParameters(){
	expect_token(TK_LPARENTHESES);
	AstParameterDeclaration parameterList;
	if(current_token.kind == TK_RPARENTHESES){
		CREATE_AST_NODE(parameterList, ParameterDeclaration);	
		EXPECT(TK_RPARENTHESES);
		return parameterList;
	}
	parameterList = ParseParameterList();
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
		AstSpecifiers specs;
		CREATE_AST_NODE(specs, Specifiers); 
		specs->tySpecs = ParseType();
		parameterList->specs = specs;
		// parameterList->specs = ParseType();
		parameterList->dec = NULL;
		parameterList->next = NULL;
	}	

	return parameterList;
}

/**
 * FunctionName = identifier .
 */
AstNode ParseFunctionName(){

	// todo 怎么确保这个位置的token是TK_ID？
	// AstExpression identifier = ParseExpression();	
	// AstExpression identifier = ParsePrimaryExpr();	
	// todo 直接使用ParseOperandName能避免使用IsPostfix，但是，合适吗？
	// AstExpression identifier = ParseOperandName();
	// TODO 以后再重构。我不想在现在做这些复制粘贴的工作，拖慢进度。
	// 语法解析和词法解析，这些工作，恶心。
	AstExpression identifier = ParseIdentifier();
	AstDeclarator functionName;
	CREATE_AST_NODE(functionName, Declarator);
	// TODO functionName->id的空间不够，可能会擦除其他数据。
	// functionName->id = (char *)malloc(sizeof(char));
	// strcpy(functionName->id, (char *)identifier->val.p);

	// TODO len应该加1吗？把字符串末尾的\0的长度也统计进来。
	int len = strlen(identifier->val.p);
	functionName->id = (char *)malloc(sizeof(char) * len);
	memset(functionName->id, 0, len);
	strcpy(functionName->id, (char *)identifier->val.p);

	return (AstNode)functionName;
}

// todo 不要。
AstNode ParseSignature(){

}

/**
 * Block = "{" StatementList "}" .
StatementList = { Statement ";" } .
 */
AstBlock ParseFunctionBody(){
	AstBlock block;
	CREATE_AST_NODE(block, Block);
	block->stmt = NULL;

	AstStatement currentStmt = NULL;

	EXPECT(TK_LBRACE);
	while(current_token.kind != TK_RBRACE){
		NO_TOKEN;
		AstStatement next = ParseStatement();
		if(block->stmt == NULL){
			currentStmt = next;
			block->stmt = currentStmt;
		}else{
			currentStmt->next = next;
			currentStmt = next;	
		}
		expect_semicolon;
	}
	EXPECT(TK_RBRACE);
	expect_semicolon;

	return block;
}

// AstStatement ParseFunctionBody(){
// 	AstNode body = NULL;
// 	// 因为函数体是可选的。
// 	if(current_token.kind == TK_LBRACE){
// 		EXPECT(TK_LBRACE);
// 	 	body = (AstNode)ParseCompoundStatement(); 	
// 		EXPECT(TK_RBRACE);
// 	}
// 	return body;
// }

// todo 返回值的数据类型应该用AstStatement吗？
// AstNode ParseFunctionBody(){
// TODO 移动到了stmt.c中，择机删除这些代码。
// AstStatement ParseFunctionBody(){
// 	// 找这个函数和{的代号，花了点时间。若经常用，花点时间记下来是不是能加快速度？
// 	EXPECT(TK_LBRACE);	
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
// 	compoundStmt->decls = decHead->next;
// 
// 	// 处理函数体中的语句。
// 	AstStatement headStmt;
// 	CREATE_AST_NODE(headStmt, Statement);
// 	headStmt->next = NULL;	
// 	// 当前token属于语句字符集 && 不是结束符 && 不是}
// 	AstStatement preStmt;
// 	AstStatement curStmt;
// 	while(current_token.kind != TK_RBRACE){
// 		// todo 未实现。
// 		curStmt = ParseStatement();
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
// 	}
// 	compoundStmt->stmt = headStmt->next;
// 
// 	EXPECT(TK_RBRACE);	
// 
// 	return compoundStmt;
// }


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
		printf("rdata-type%d = %s\n", rcount, ((AstTypedefName)rdec->specs->tySpecs)->id);
		char return_value[10];
		if((AstParameterDeclaration)(rdec)->dec == NULL || ((AstParameterDeclaration)(rdec))->dec->id == 0){
//			*return_value = "no-return";
			strcpy(return_value, "no-return");
		}else{
			strcpy(return_value, ((AstParameterDeclaration)(rdec))->dec->id);
		}
		printf("return%d = %s\n", rcount, return_value);
		rdec = (AstParameterDeclaration)rdec->next;
		rcount++;
	}
}

/**
 * FunctionDecl = "func" FunctionName Signature [ FunctionBody ] .
 */
// AstNode ParseFunctionDecl(){
AstFunction ParseFunctionDecl(){

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
	
	AstParameterTypeList signature;
	CREATE_AST_NODE(signature, ParameterTypeList);
	AstParameterDeclaration result = ParseResult(); 
	signature->paramDecls = result;
	fdec->sig = signature;
	
	// todo 测试，打印数据。
//	PrintFdec(fdec);
	
		
	AstFunction func;
	CREATE_AST_NODE(func, Function);
	func->fdec = fdec;
	// return (AstNode)func;

	// 处理FunctionBody
	AstBlock block = ParseFunctionBody();
	// CREATE_AST_NODE(stmt, Statement);
	func->block = block;
	
	return func;
}

int CurrentTokenIn(int toks[]){
	int *p = toks;
	
	while(*p != TOKEN_SET_END_FLAG){
		if(*p == current_token.kind){
			return 1;
		}
		p++;
	}

	return 0;
}

/**
 * ShortVarDecl = IdentifierList ":=" ExpressionList .
 */
AstShortVarDecl ParseShortVarDecl(){
	AstShortVarDecl shortVarDecl;
	CREATE_AST_NODE(shortVarDecl, ShortVarDecl);

	shortVarDecl->identifierList = ParseIdentifierList();
	EXPECT(TK_INIT_ASSIGN);
	shortVarDecl->expressionList = ParseExpressionList();	

	expect_semicolon;
	
	return shortVarDecl;
}

/**
 * MethodDecl = "func" Receiver MethodName Signature [ FunctionBody ] .
 * Receiver   = Parameters .
 * FunctionDecl = "func" FunctionName Signature [ FunctionBody ] .
 *
 * Signature      = Parameters [ Result ] .
Result         = Parameters | Type .
Parameters     = "(" [ ParameterList [ "," ] ] ")" .
ParameterList  = ParameterDecl { "," ParameterDecl } .
ParameterDecl  = [ IdentifierList ] [ "..." ] Type .
 */
// AstNode ParseMethodDecl(){
AstMethodDeclaration ParseMethodDecl(){

	NEXT_TOKEN;

	AstFunctionDeclarator dec;
	CREATE_AST_NODE(dec, FunctionDeclarator);

	// Receiver
	AstParameterTypeList paramTypeList2;
	CREATE_AST_NODE(paramTypeList2, ParameterTypeList);
	AstParameterDeclaration params2 = ParseParameters(); 
	paramTypeList2->paramDecls = params2;
	dec->receiver = paramTypeList2;

	AstNode functionName = ParseFunctionName();

	AstParameterTypeList paramTypeList;
	CREATE_AST_NODE(paramTypeList, ParameterTypeList);
	AstParameterDeclaration params = ParseParameters(); 
	paramTypeList->paramDecls = params;
	dec->paramTyList = paramTypeList;
	
	AstParameterTypeList signature;
	CREATE_AST_NODE(signature, ParameterTypeList);
	AstParameterDeclaration result = ParseResult(); 
	signature->paramDecls = result;
	dec->sig = signature;
		
//	AstFunction func;
//	CREATE_AST_NODE(func, Function);
//	func->fdec = fdec;
	// return (AstNode)func;
	AstMethodDeclaration methodDeclaration;
	CREATE_AST_NODE(methodDeclaration, MethodDeclaration);
	methodDeclaration->fdec = dec;

	// 处理FunctionBody
	AstBlock block = ParseFunctionBody();

	methodDeclaration->block = block;

	return methodDeclaration;
}

AstNode ParseMethodDeclOrFunctionDecl(){
	unsigned char type = 0;
	StartPeekToken();	
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		type = 1;
	}
	EndPeekToken();	
	
	AstNode node;

	if(type == 0){
		node = (AstNode)ParseFunctionDecl();
	}else{
		node = (AstNode)ParseMethodDecl();
	}

	return node;
}
