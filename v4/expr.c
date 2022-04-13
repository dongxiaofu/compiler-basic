#include "lex.h"
#include "ast.h"
#include "decl.h"
#include "expr.h"

/**
 * ExpressionList = Expression { "," Expression } .
 *
 */
AstExpression ParseExpressionList(){
	LOG("%s\n", "parse ExpressionList");

	// todo 创建一个链表，竟然还要想比较长时间。
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	AstExpression curExpr;
	CREATE_AST_NODE(curExpr, Expression); 
	AstExpression preExpr;
	CREATE_AST_NODE(preExpr, Expression); 
	
	int count = 0;
	expr = ParseExpression();
	// todo 有问题吗？我希望的效果是：解析出一个表达式后就统计一次次数。
	count++;
	preExpr = expr;
	while(current_token.kind==TK_COMMA){
		NEXT_TOKEN;
//		memset(curExpr, 0, sizeof(*curExpr));
		curExpr = ParseExpression();
		curExpr->next = NULL;
		count++;
		preExpr->next = (AstNode)curExpr;
		preExpr = curExpr;
	}
	// preExpr->next = NULL;
	
	expr->variable_count = count;

	return expr;
}

/**
 * Expression = UnaryExpr | Expression binary_op Expression .
UnaryExpr  = PrimaryExpr | unary_op UnaryExpr .

binary_op  = "||" | "&&" | rel_op | add_op | mul_op .
rel_op     = "==" | "!=" | "<" | "<=" | ">" | ">=" .
add_op     = "+" | "-" | "|" | "^" .
mul_op     = "*" | "/" | "%" | "<<" | ">>" | "&" | "&^" .

unary_op   = "+" | "-" | "!" | "^" | "*" | "&" | "<-" .

产生式的文档在：https://golang.org/ref/spec#Operators

 */
AstExpression ParseExpression(){
	LOG("%s\n", "parse Expression");
//	ParseUnaryExpr() || ParseBinaryExpr();
//	if(TK_POSITIVE <= current_token.kind && current_token.kind <= TK_RECEIVE){
//	if(IsUnaryOp() == 1){
//		ParseUnaryExpr();
//	}else{
//		Token pre_token = current_token;
//		
//
//		ParseBinaryExpr();
//
//	}
	AstExpression expr;
//	CREATE_AST_NODE(expr, Expression);
	// expr = ParseBinaryExpr(4);
	// todo 参数prec如何确定？
//	expr = ParseBinaryExpr(2);
	// expr = ParseBinaryExpr(7);
	// OP_CONDITIONAL_OR 是优先级最低的二元运算符号。
	if(current_token.kind == TK_ADD || current_token.kind == TK_MINUS || current_token.kind == TK_MUL){
		expr = ParseUnaryExpr();
	}else{
		expr = ParseBinaryExpr(Prec[OP_CONDITIONAL_OR]);
//		expr = ParseBinaryExpr(5);
	}

//	 if(CurrentTokenIn(FIRST_Expression) == 1){
//		expr = ParseBinaryExpr(Prec[OP_CONDITIONAL_OR]);
//	}else{
//		expr = ParseUnaryExpr();
//	}
//	expr = ParseBinaryExpr(4);

	return expr;
}

// todo 这个函数难理解。
AstExpression ParseBinaryExpr(int prec){
	LOG("%s\n", "parse BinaryExpr");
//	ParseExpression();
//	// todo 如何处理binary_op？
//	ParseBinaryOp();
//	ParseExpression();
	// #define HIGHEST_BIN_PREC Prec[TK_MUL - TK_CONDITIONAL_OR]	
	// OP_MUL是优先级最高的二元运算符号。
	#define HIGHEST_BIN_PREC Prec[OP_MUL]	

	AstExpression binExpr;
	AstExpression expr;

	CREATE_AST_NODE(binExpr, Expression);
	CREATE_AST_NODE(expr, Expression);

	//dump_token_number();	

	if(prec == HIGHEST_BIN_PREC){
		expr = ParseUnaryExpr();
	}else{
	//	binExpr->op = BINARY_OP;
	//	binExpr->kids[0] = expr;
	//	NEXT_TOKEN;
	//	binExpr->kids[1] = ParseBinaryExpr(prec + 1);
	//	expr = binExpr;
		expr = ParseBinaryExpr(prec + 1);
//		ParseBinaryExpr(prec + 1);
	}

	while(IsBinaryOp() == 1 && Prec[BINARY] == prec){
//		AstExpression binExpr;
//		AstExpression expr;

//		CREATE_AST_NODE(binExpr, Expression);
//		CREATE_AST_NODE(expr, Expression);

		NEXT_TOKEN;
		if(prec == HIGHEST_BIN_PREC){
			expr = ParseUnaryExpr();
		}else{
			binExpr->op = BINARY_OP;
			binExpr->kids[0] = expr;
//			NEXT_TOKEN;
			// binExpr->kids[1] = ParseBinaryExpr(prec + 1);
			ParseBinaryExpr(prec + 1);
			expr = binExpr;
		}
	}

	return expr;
}
/**
 * UnaryExpr  = PrimaryExpr | unary_op UnaryExpr .
 * unary_op   = "+" | "-" | "!" | "^" | "*" | "&" | "<-" .
 */
AstExpression ParseUnaryExpr(){
	//dump_token_number();	

//	LOG("%s\n", "parse UnaryExpr");
//	AstExpression expr;	
//	CREATE_AST_NODE(expr, Expression);
//	// if(TK_POSITIVE <= current_token.kind && current_token.kind <= TK_RECEIVE){
//	// TODO IsUnaryOp 需要重构。有可能根本就不正确。
//	// if(IsUnaryOp() == 1){
//	char flag = 0;
//	if(flag == 1){
//		expr->op = UNARY_OP;
//		NEXT_TOKEN;
//		expr->kids[0] = ParseUnaryExpr();
//	}else{
//		expr = ParsePrimaryExpr();
//	}

	LOG("%s\n", "parse UnaryExpr");
	AstExpression expr;	
	CREATE_AST_NODE(expr, Expression);

	switch (current_token.kind){
		case TK_ADD:			// +
		case TK_MINUS:			// -
		case TK_NOT:			// !
		case TK_BINARY_BITWISE_XOR:	// ^
		case TK_MUL:			// *
		case TK_BITWISE_AND:		// &
		case TK_RECEIVE:		// <-
			expr->op = UNARY_OP;
			NEXT_TOKEN;
			expr->kids[0] = ParseUnaryExpr();
			break;
		default:
			expr = ParsePrimaryExpr();
	}
	return expr;
}

// Conversion = Type "(" Expression [ "," ] ")" .
AstNode ParseConversion(){
	// NEXT_TOKEN;
	ParseType();
	expect_token(TK_LPARENTHESES);
	ParseExpression();
	expect_comma;
	expect_token(TK_RPARENTHESES);
}

// todo 暂时不处理后缀表达式。
int IsPostfix(TokenKind kind){
//	return 0;
	return (kind == TK_DOT || kind == TK_LBRACKET || kind == TK_LPARENTHESES);
	// return (kind == TK_DOT || kind == TK_LBRACKET);
} 

/**
 * Selector       = "." identifier .
 * TypeAssertion  = "." "(" Type ")" .
 */
AstNode ParseSelectorTypeAssertion(){
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		NEXT_TOKEN;
		ParseType();
	//	ExpectDataType();
		expect_token(TK_RPARENTHESES);
	}else{
	//	expect_token(TK_ID);
		ParseIdentifier();
	}

	AstNode node;
	CREATE_AST_NODE(node, Node);

	return node;
}

/**
 * Arguments      = "(" [ ( ExpressionList | Type [ "," ExpressionList ] ) [ "..." ] [ "," ] ] ")" .
 * ()
 * (abc, def)
 * (uint abc, def,name)
 * (uint abc, def,name) ...
 */
AstNode ParseArguments(){
	expect_token(TK_LPARENTHESES);

	AstNode node;
	CREATE_AST_NODE(node, Node);

	if(current_token.kind == TK_ELLIPSIS){
		NEXT_TOKEN;
	}

	if(current_token.kind == TK_COMMA){
		NEXT_TOKEN;
		return node;
	}

	// TODO 这段代码可以简化。 
	if(CurrentTokenIn(FIRST_Expression) == 1){
		ParseExpressionList();
	}else{
		ParseType();
		if(current_token.kind == TK_COMMA){
			NEXT_TOKEN;
			ParseExpressionList();
		}
	}

	if(current_token.kind == TK_ELLIPSIS){
		NEXT_TOKEN;
	}

	if(current_token.kind == TK_COMMA){
		NEXT_TOKEN;
	}

	expect_token(TK_RPARENTHESES);
	
	return node;
}

// /**
//  * Arguments      = "(" [ ( ExpressionList | Type [ "," ExpressionList ] ) [ "..." ] [ "," ] ] ")" .
//  * ()
//  * (abc, def)
//  * (uint abc, def,name)
//  * (uint abc, def,name) ...
//  */
// AstNode ParseArguments(){
// 	expect_token(TK_LPARENTHESES);
// 	
// 	while(current_token.kind != TK_RPARENTHESES){
// 		if(IsDataType(current_token.value.value_str) == 1){
// 			NEXT_TOKEN;
// 		}else{
// 			ParseExpressionList();
// 		}
// 
// 		// 第一个循环
// 		while(current_token.kind == TK_COMMA){
// 			NEXT_TOKEN;
// 			ParseExpressionList();
// 		}
// 
// 		// 第二个循环
// 		while(current_token.kind == TK_ELLIPSIS){
// 			NEXT_TOKEN;
// 		}
// 
// 		// 第三个循环
// 		while(current_token.kind == TK_COMMA){
// 			NEXT_TOKEN;
// 		}
// 	}	
// 
// 	expect_token(TK_RPARENTHESES);
// 
// 	AstNode node;
// 	CREATE_AST_NODE(node, Node);
// 
// 	return node;
// }

/**
 * Index          = "[" Expression "]" .
Slice          = "[" [ Expression ] ":" [ Expression ] "]" |
                 "[" [ Expression ] ":" Expression ":" Expression "]" .
 */
AstNode ParseIndexSlice(){
	NEXT_TOKEN;

	AstNode node;
	CREATE_AST_NODE(node, Node);

	if(current_token.kind != TK_COLON){
		ParseExpression();
	}

	if(current_token.kind == TK_RBRACKET){
		NEXT_TOKEN;
		return node;
	}

	EXPECT(TK_COLON);

	if(current_token.kind == TK_RBRACKET){
		NEXT_TOKEN;
		return node;
	}
	
	ParseExpression();
	if(current_token.kind == TK_COLON){
		EXPECT(TK_COLON);
		ParseExpression();
	}
	EXPECT(TK_RBRACKET);

	return node;
}

// /**
//  * Index          = "[" Expression "]" .
// Slice          = "[" [ Expression ] ":" [ Expression ] "]" |
//                  "[" [ Expression ] ":" Expression ":" Expression "]" .
//  */
// AstNode ParseIndexSlice(){
// 	NEXT_TOKEN;
// 	
// 	int expr_count = 0;
// 	// TokenKind kind = current_token.kind;	
// 	TokenKind kind  = current_token.kind;	
// 	while(kind != TK_RBRACKET && kind != TK_COLON){ 	
// 		NO_TOKEN;
// 		ParseExpression();
// 		kind  = current_token.kind;
// 		expr_count++;
// 	}
// 
// 	// if(expr_count == 1 && kind == TK_RBRACKET){
// 	if(kind == TK_RBRACKET){
// 		printf("%s\n", "It is a Index");
// 		expect_token(TK_RBRACKET);
// 		 // NEXT_TOKEN;
// 	//	expect_token();
// 
// 	}else if(kind == TK_COLON){
// 		printf("%s\n", "It is a Slice");
// 		NEXT_TOKEN;
// 
// 		int expr_count = 0;
// 		kind = current_token.kind;	
// 		// while(kind != TK_COLON){ 	
// 		while(kind != TK_RBRACKET && kind != TK_COLON){ 	
// 			NO_TOKEN;
// 			ParseExpression();
// 			kind = current_token.kind;	
// 			expr_count++;
// 		}
// 
// 		if(kind == TK_COLON){
// 			NEXT_TOKEN;
// 			ParseExpression();
// 			// expect_token(TK_RBRACKE);
// 		}else{
// 			// NEXT_TOKEN;
// 			// todo 可以去掉这个else
// 		}
// 		expect_token(TK_RBRACKET);
// 	}else{
// 		ERROR("ParseIndexSlice error\n");
// 	}
// }

/**
 * PrimaryExpr =
	Operand |
	Conversion |
	MethodExpr |
	PrimaryExpr Selector |
	PrimaryExpr Index |
	PrimaryExpr Slice |
	PrimaryExpr TypeAssertion |
	PrimaryExpr Arguments .

Selector       = "." identifier .
Index          = "[" Expression "]" .
Slice          = "[" [ Expression ] ":" [ Expression ] "]" |
                 "[" [ Expression ] ":" Expression ":" Expression "]" .
TypeAssertion  = "." "(" Type ")" .
Arguments      = "(" [ ( ExpressionList | Type [ "," ExpressionList ] ) [ "..." ] [ "," ] ] ")" .
 */
AstExpression ParsePrimaryExpr(){
	LOG("%s\n", "parse PrimaryExpr");

//	//dump_token_number();	

	AstExpression expr;	
	CREATE_AST_NODE(expr, Expression);
	if(IsDataType(current_token.value.value_str) == 1){
		ParseConversion();
	}else{
//		NEXT_TOKEN;
		// todo 解析 Operand、MethodExpr。目前，只解析Operand。
		// 区分Operand和MethodExpr。
		// 1：MethodExpr；0：Oprand。
		unsigned char type = 0;
		StartPeekToken();
		if(current_token.kind == TK_ID){
//			ParseTypeName();
//			ParseOperandName();
			ParseTypeName();
			if(current_token.kind == TK_DOT){
				type = 1;
			}
		}else if(current_token.kind == TK_LPARENTHESES){
			NEXT_TOKEN;
			if(CurrentTokenIn(FIRST_Expression) == 0){
				type = 1;
			}
		}else if(current_token.kind == TK_FUNC){
			NEXT_TOKEN;
			ParseParameters();
			ParseResult();		
			if(current_token.kind != TK_LBRACE){
				type = 1;
			}
		}else if(current_token.kind == TK_STRING){
			// do nothing
		
		}else if(current_token.kind == TK_ELLIPSIS){
			// do nothing
		}else if(isTypeKeyWord(current_token.kind) == 1){
			ParseType();
			// TODO 其实可以不必这么写。这样写，逻辑更清晰。
			if(current_token.kind == TK_LBRACE){
				type = 0;	
			}else{
				type = 1;
			}
		}

		EndPeekToken();
		if(type == 1){
			expr = ParseMethodExpr();
		}else{
			expr = ParseOperand();
		}
	}

	// TODO 不理解这里的逻辑，参考ucc的ParsePostfixExpression函数写的。
	while(1){
		switch(current_token.kind){
			case TK_DOT:
				ParseSelectorTypeAssertion();
				break; 
			case TK_LPARENTHESES:
				ParseArguments();
				break; 
			case TK_LBRACKET:
				ParseIndexSlice();
				break; 
			default:
//				break;
				return expr;
		}		
	}

//	return expr;
}

AstNode ParseBinaryOp(){
	LOG("%s\n", "parse binary_op");
	// return (TK_CONDITIONAL_OR <= current_token.kind && current_token.kind <= TK_BITWISE_AND_NOT);
	if(TK_CONDITIONAL_AND <= current_token.kind && current_token.kind <= TK_BITWISE_AND_NOT){
		NEXT_TOKEN;
	}else{
		// TODO 这是啥意思？
//		ParseRelOp() || ParseAddOp() || ParseMulOp(); 
		ERROR("expect a binary op, but give %d\n", current_token.kind);
	}
}


AstNode ParseRelOp(){
	LOG("%s\n", "parse rel_op");
	if(TK_EQUAL <= current_token.kind && current_token.kind <= TK_GREATER_OR_EQUAL){
		NEXT_TOKEN;
	}else{
		ERROR("expect a real op, but give %d\n", current_token.kind);
	}
}


AstNode ParseAddOp(){
	LOG("%s\n", "parse add_op");
	if(TK_ADD <= current_token.kind && current_token.kind <= TK_BINARY_BITWISE_XOR){
		NEXT_TOKEN;
	}else{
		ERROR("expect a add op, but give %d\n", current_token.kind);
	}
}


AstNode ParseMulOp(){
	LOG("%s\n", "parse mul_op");
	if(TK_MUL <= current_token.kind && current_token.kind <= TK_BITWISE_AND_NOT){
		NEXT_TOKEN;
	}else{
		ERROR("expect a mul op, but give %d\n", current_token.kind);
	}
}

int IsRelOp(){
	return (TK_EQUAL <= current_token.kind && current_token.kind <= TK_GREATER_OR_EQUAL);
}

int IsAddOp(){
	return (TK_ADD <= current_token.kind && current_token.kind <= TK_BINARY_BITWISE_XOR);
}

int IsMulOp(){
	return (TK_MUL <= current_token.kind && current_token.kind <= TK_BITWISE_AND_NOT);
}

int IsUnaryOp(){
	return (TK_EQUAL <= current_token.kind && current_token.kind <= TK_GREATER_OR_EQUAL);
}

int IsBinaryOp(){
	return (TK_CONDITIONAL_OR <= current_token.kind && current_token.kind <= TK_BITWISE_AND_NOT);
//	return ((TK_CONDITIONAL_AND <= current_token.kind && current_token.kind <= TK_CONDITIONAL_OR)
//		|| (IsRelOp() == 1)
//		|| (IsAddOp() == 1)
//		|| (IsMulOp() == 1)
	//	|| current_token.kind == TK_POSITIVE
	//	|| current_token.kind == TK_NEGATIVE
	//	|| current_token.kind == TK_POINTER
	//	|| current_token.kind == TK_ADDRESS
//		);
}

/**
 * Operand     = Literal | OperandName | "(" Expression ")" .
Literal     = BasicLit | CompositeLit | FunctionLit .
BasicLit    = int_lit | float_lit | imaginary_lit | rune_lit | string_lit .
OperandName = identifier | QualifiedIdent .
 */
AstExpression ParseOperand(){
	// dump_token_number();	
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);

	/**
 	 *
     * 怎么想出这种逻辑？
     * 测试下面的代码，补充了这些逻辑。goto 挺有用的。
     * dData := DivideError{
                    dividee: varDividee,
                    divider: varDivider,
            }
     */
	if(current_token.kind == TK_ID){

		// TODO 这里，暂时不能要。需要进行语义分析才能解析上面的dData例程。暂时注释。
//		StartPeekToken();
//		expr = ParseOperandName();
//		if(current_token.kind == TK_LBRACE){
//			EndPeekToken();
//			goto literal;
//		}
//		EndPeekToken();
	
		expr = ParseOperandName();
	}else if(current_token.kind == TK_LPARENTHESES ){
		EXPECT(TK_LPARENTHESES);
		expr = ParseExpression();
		EXPECT(TK_RPARENTHESES);
	}else{
literal:
		expr = ParseLiteral();
	}

	return expr;
}

AstExpression ParseIntLit(){
	// todo 不完善。
//	// todo 应该使用malloc分配内存空间才更妥当吗？
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	if(current_token.kind == TK_NUM){
		expr = ParseBasicLit();
	}
	
	return expr;
}

/**
 * BasicLit    = int_lit | float_lit | imaginary_lit | rune_lit | string_lit .
 */
// TODO 这个函数很复杂，工作量非常大，后期再处理。
AstExpression ParseBasicLit(){
//	// todo 应该使用malloc分配内存空间才更妥当吗？
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	if(current_token.kind == TK_NUM){
		// expr->op = OP_CONST;
		// TODO 这是不正确的。临时这样做。
		expr->op = OP_NONE;
		union value v = {current_token.value.value_num,0};
		expr->val = v;
		NEXT_TOKEN;
	}else if(current_token.kind == TK_STRING){
		// TODO 可能不正确。临时这样做。
		expr->op = OP_STR;
		expr->val.p = (char *)malloc(sizeof(char) * MAX_NAME_LEN);;
		strcpy(expr->val.p, current_token.value.value_str);
		NEXT_TOKEN;
	}

	return expr;
}

// 不用写注释吧？
unsigned char IsCompositeLit(){
	if(current_token.kind == TK_STRING){
		return 0;
	}

	if(current_token.kind == TK_FUNC){
		return 0;
	}

	// 为啥不写出用||连接的语句？为了书写方便。
	if(current_token.kind == TK_MAP){
		return 1;
	}

	if(current_token.kind == TK_ID){
		return 1;
	}

	if(current_token.kind == TK_SLICE){
		return 1;
	}

 	if(current_token.kind == TK_STRUCT){
		return 1;
	}

	if(current_token.kind == TK_LBRACKET){
		return 1;
	}

	return 0;
}

/**
 * Literal     = BasicLit | CompositeLit | FunctionLit .
 * BasicLit    = int_lit | float_lit | imaginary_lit | rune_lit | string_lit .
 *
 */
AstExpression ParseLiteral(){
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);

	// 怎么识别CompositeLit？
	// CompositeLit  = LiteralType LiteralValue .
	// 只需判断LiteralType的类型。
	// 第一个token是TK_STRUCT、TK_MAP、TK_LBRACE中的第一个即可。
	// 当然，这是在本函数中，是有具体上下文的。

	if(current_token.kind == TK_FUNC){
		expr = ParseFunctionLit();
	}else if(IsCompositeLit() == 1){
		expr = ParseCompositeLit(); 
	}else{
		expr = ParseBasicLit();
	}

	return expr;
}

/**
 * OperandName = identifier | QualifiedIdent .
 * QualifiedIdent = PackageName "." identifier .
 * PackageName    = identifier .
 */
AstExpression ParseOperandName(){
	char type = -1;
	StartPeekToken();
	// 最多只需要遍历三个token。
	for(unsigned char i = 0; i < 3; i++){
		if(i == 1 && current_token.kind == TK_DOT){
			type = 2;
			break;
		}
		type = 1;
	}
	EndPeekToken();

 	AstExpression expr;
 	CREATE_AST_NODE(expr, Expression);

	if(type == 1){
		AstExpression expr1 = ParseIdentifier();
		if(expr == NULL){
			// TODO decl为空时，expr不是有意义的数据，直接返回可以吗？但我不知道应该怎么做。 			
			return expr;
		}
 		expr->op = OP_ID;
 		expr->val.p = (void *)malloc(sizeof(char) * MAX_NAME_LEN);
 		strcpy((char *)(expr->val.p), expr->val.p);
		return expr;
	}else if(type == 2){
		ParseQualifiedIdent();
	}else{
		expect_token(TK_ID);
	}
}

/**
 * FunctionLit = "func" Signature FunctionBody .
 */
AstFunctionLit ParseFunctionLit(){

	EXPECT(TK_FUNC);

	AstParameterDeclaration paramTyList = ParseParameters();
	AstParameterDeclaration result = ParseResult();
	AstBlock body = ParseFunctionBody();

	AstFunctionDeclarator fdecl;
	CREATE_AST_NODE(fdecl, FunctionDeclarator);
	fdecl->paramTyList = paramTyList;
	fdecl->sig = result;

	AstFunctionLit functionLit;
	CREATE_AST_NODE(functionLit, FunctionLit);

	functionLit->fdecl = fdecl;
	functionLit->body = body;

	return functionLit;
}

/**
 * CompositeLit  = LiteralType LiteralValue .
 */
// TODO 没有建立AST。
AstNode ParseCompositeLit(){
	AstNode node;
	CREATE_AST_NODE(node, Node);
	
	ParseLiteralType();
	ParseLiteralValue();	
 
	return node;
}

/**
 * LiteralValue  = "{" [ ElementList [ "," ] ] "}" .
ElementList   = KeyedElement { "," KeyedElement } .
KeyedElement  = [ Key ":" ] Element .
Key           = FieldName | Expression | LiteralValue .
FieldName     = identifier .
Element       = Expression | LiteralValue .
 */
// TODO 没有组建AST
// 创建层层分解的函数，代码变得简洁，复杂度降低，可是建立AST时可能会有许多不方便。怎么办？
AstNode ParseLiteralValue(){
	AstNode node;
	CREATE_AST_NODE(node, Node);
	
	EXPECT(TK_LBRACE);
	ParseElementList();
	expect_comma;
	EXPECT(TK_RBRACE);

	return node;
}

// TODO 没有组建AST
AstNode ParseElementList(){
	AstNode node;
	CREATE_AST_NODE(node, Node);

	// TODO 不喜欢这样写。解析此段代码还用到了外部条件。
	// while(current_token.kind != TK_COMMA || current_token.kind != TK_RBRACE){
	while(current_token.kind != TK_RBRACE){
		if(current_token.kind == TK_COMMA)	continue;
		ParseKeyedElement();
		// 处理逗号
		NEXT_TOKEN;
	}

//	ParseKeyedElement();
//	while(current_token.kind == TK_COMMA){
//		NEXT_TOKEN;
//		ParseKeyedElement();
//	}

	return node;
}

/**
 * KeyedElement  = [ Key ":" ] Element .
Key           = FieldName | Expression | LiteralValue .
FieldName     = identifier .
Element       = Expression | LiteralValue .
 */
// TODO 没有建立AST。
AstNode ParseKeyedElement(){

	// 这是唯一的难点。
	unsigned char flag = 0;
//	StartPeekToken();
//	// while(current_token.kind == TK_RBRACE){
//	while(current_token.kind != TK_COMMA){
//		if(current_token.kind == TK_COLON){
//	//		flag = 1;
//			break;
//		}
//
//		NEXT_TOKEN;
//	}
//	EndPeekToken();

	// Key           = FieldName | Expression | LiteralValue .
//	if(flag == 1){
		if(current_token.kind == TK_LBRACE){
			ParseLiteralValue();
		}else{
			if(CurrentTokenIn(FIRST_Expression) == 1){
				ParseExpression();
			}else{	// FieldName
				ParseIdentifier();
			}	
		}

		// 处理:
//		EXPECT(TK_COLON);
		if(current_token.kind == TK_COLON){
			flag = 1;
			EXPECT(TK_COLON);
		}
//	}
	AstNode node;
	CREATE_AST_NODE(node, Node);
	if(flag == 0){
		return node;
	}

	if(current_token.kind == TK_LBRACE){
		ParseLiteralValue();
	}else{
		ParseExpression();
	}

//	AstNode node;
//	CREATE_AST_NODE(node, Node);

	return node;
}


/**
 * MethodExpr    = ReceiverType "." MethodName .
ReceiverType  = Type .
MethodName         = identifier .
 */
AstExpression ParseMethodExpr(){

	ParseType();
	EXPECT(TK_DOT);
	ParseIdentifier();

	AstNode node;
	CREATE_AST_NODE(node, Node);

	return node;
}

// TODO 这个函数很复杂，工作量非常大，后期再处理。

// AstExpression ParseOperandName(){
// 	//dump_token_number();	
// 	AstExpression expr;
// 	CREATE_AST_NODE(expr, Expression);
// 	if(current_token.kind == TK_ID){
// 		// TODO 这是不正确的。临时这样做。
// 		expr->op = OP_NONE;
// 		expr->val.p = (void *)malloc(sizeof(char) * MAX_NAME_LEN);
// 		strcpy((char *)(expr->val.p), current_token.value.value_str);
// 		NEXT_TOKEN;
// 	}
// 
// 	return expr;
// }


// AstExpression ParseDecimalDigit(){
// 
// }
// AstExpression ParseDecimalDigits(){
// 
// 
// }
// AstExpression ParseDecimalLit(){
// 
// }
