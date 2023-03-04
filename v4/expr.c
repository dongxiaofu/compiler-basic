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
	AstExpression expr = ParseBinaryExpr(Prec[OP_CONDITIONAL_OR]);

	return expr;
}

AstExpression ParseFactor()
{
	AstExpression expr = ParseUnaryExpr();
	return expr;
}

AstExpression ParseTerm()
{
	AstExpression binExpr = NULL;
//	CREATE_AST_NODE(binExpr, Expression);
	AstExpression expr = NULL;
	int flag = 0;
	do{
		AstExpression cur;
		// CREATE_AST_NODE(cur, Expression);
		if(current_token.kind == TK_MUL){
			CREATE_AST_NODE(cur, Expression);
			cur->op = BINARY_OP;
			cur->kids[0] = expr;
			flag = 1;
			NEXT_TOKEN;
		}
		expr = ParseFactor();
		if(flag == 1){
			cur->kids[1] = expr;
		//	if(binExpr == NULL){
		//		binExpr = cur;
		//	}
			binExpr = cur;
			expr = cur;	
		}else{
			if(binExpr == NULL){
			//	binExpr = expr;
			}
		}
	}while(current_token.kind == TK_MUL);

	if(binExpr == NULL){
		binExpr = expr;
	}
	return binExpr;
}

AstExpression ParseBinaryExpr2(int prec){
	AstExpression binExpr = NULL;
//	CREATE_AST_NODE(binExpr, Expression);
	AstExpression expr = NULL;
	int flag = 0;
//	while(1){
		do{
			AstExpression cur;
			// CREATE_AST_NODE(cur, Expression);
			if(current_token.kind == TK_ADD){
				CREATE_AST_NODE(cur, Expression);
				NEXT_TOKEN;
				cur->kids[0] = expr;
				flag = 1;
			}
			expr = ParseTerm();	
			if(flag == 1){
				cur->kids[1] = expr;
//				if(binExpr == NULL){
//					binExpr = cur;
//				}
				binExpr = cur;
				expr = cur;
			//	expr = binExpr;
			}else{
			//	cur = expr;
//				binExpr = cur;
			}
			
		}while(current_token.kind == TK_ADD);
//	}
	
//	binExpr = expr;
	if(binExpr == NULL){
		binExpr = expr;
	}

	return binExpr;
}

// todo 这个函数难理解。
AstExpression ParseBinaryExpr(int prec){
	LOG("%s\n", "parse BinaryExpr");
	// OP_MUL是优先级最高的二元运算符号。
	#define HIGHEST_BIN_PREC Prec[OP_MUL]	
//	#define HIGHEST_BIN_PREC Prec[TK_MUL]	

	AstExpression binExpr;
	AstExpression expr;

	int t = HIGHEST_BIN_PREC;
	int b = 4;

//	CREATE_AST_NODE(binExpr, Expression);
//	CREATE_AST_NODE(expr, Expression);

	if(prec == HIGHEST_BIN_PREC){
		expr = ParseUnaryExpr();
	}else{
		expr = ParseBinaryExpr(prec + 1);
	}

	while(IsBinaryOp() == 1 && Prec[BINARY] == prec){
		CREATE_AST_NODE(binExpr, Expression);
		int op = BINARY_OP;
		if(op == OP_ASSIGN){
			t = 5;
		}
		binExpr->op = op;
		binExpr->kids[0] = expr;
		NEXT_TOKEN;
		if(prec == HIGHEST_BIN_PREC){
//			expr = ParseUnaryExpr();
			binExpr->kids[1] = ParseUnaryExpr();
		}else{
//			binExpr->op = BINARY_OP;
	//		binExpr->kids[0] = expr;
			binExpr->kids[1] = ParseBinaryExpr(prec + 1);
		//	expr = binExpr;
		}
		expr = binExpr;
	}

	return expr;
}
/**
 * UnaryExpr  = PrimaryExpr | unary_op UnaryExpr .
 * unary_op   = "+" | "-" | "!" | "^" | "*" | "&" | "<-" .
 */
AstExpression ParseUnaryExpr(){
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
AstExpression ParseConversion(){
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	expr->op = OP_CAST;

	expr->kids[0] = (AstExpression)ParseType();

	expect_token(TK_LPARENTHESES);
	expr->kids[1] = ParseExpression();
	expect_comma;
	expect_token(TK_RPARENTHESES);

	return expr;
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
void ParseSelectorTypeAssertion(AstExpression expr){
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		NEXT_TOKEN;
		expr->op = OP_TYPE_ASSERT;
		expr->kids[1] = ParseType();
		expect_token(TK_RPARENTHESES);
	}else{
		// TODO 会不会是type.method？
		expr->op = OP_MEMBER;
		AstExpression member = ParseIdentifier();
		if(member != NULL){
			expr->val.p = member->val.p;
		}
	}
}

/**
 * Arguments      = "(" [ ( ExpressionList | Type [ "," ExpressionList ] ) [ "..." ] [ "," ] ] ")" .
 * ()
 * (abc, def)
 * (uint abc, def,name)
 * (uint abc, def,name) ...
 *
 *
 * func Greeting(prefix string, who ...string)
Greeting("nobody")
Greeting("hello:", "Joe", "Anna", "Eileen")

s := []string{"James", "Jasmine"}
Greeting("goodbye:", s...)
 */
AstArguments ParseArguments(){
	expect_token(TK_LPARENTHESES);

	AstArguments args;
	CREATE_AST_NODE(args, Arguments);

	if(current_token.kind == TK_ELLIPSIS){
		args->hasEllipsis = 1;
		NEXT_TOKEN;
	}

	if(current_token.kind == TK_COMMA){
		NEXT_TOKEN;
		return args;
	}

	// TODO 这段代码可以简化。 
	if(CurrentTokenIn(FIRST_Expression) == 1){
		args->args = ParseExpressionList();
	}else{
		// TK_RPARENTHESES 是 )
		if(current_token.kind == TK_RPARENTHESES){
			NEXT_TOKEN;
			return args;
		}
		args->type = (AstSpecifiers)ParseType();
		if(current_token.kind == TK_COMMA){
			NEXT_TOKEN;
			args->args = ParseExpressionList();
		}
	}

	if(current_token.kind == TK_ELLIPSIS){
		NEXT_TOKEN;
		args->hasEllipsis = 1;
	}

	if(current_token.kind == TK_COMMA){
		NEXT_TOKEN;
	}

	expect_token(TK_RPARENTHESES);
	
	return args;
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
	AstNode node = NULL;

	if(current_token.kind != TK_COLON){
		node = (AstNode)ParseExpression();
	}

	if(current_token.kind == TK_RBRACKET){
		NEXT_TOKEN;
		return node;
	}

	AstSliceMeta meta;
	CREATE_AST_NODE(meta, SliceMeta);
	meta->start = (AstExpression)node;

	EXPECT(TK_COLON);

	if(current_token.kind == TK_RBRACKET){
		NEXT_TOKEN;
		return (AstNode)meta;
	}
	
	meta->len = ParseExpression();
	if(current_token.kind == TK_COLON){
		EXPECT(TK_COLON);
		meta->cap = ParseExpression();
	}
	EXPECT(TK_RBRACKET);

	return (AstNode)meta;
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


AstExpression ParseDotExpr()
{
	AstExpression expr = ParseIdentifier();
	return expr;
}


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

	AstExpression expr;	
	CREATE_AST_NODE(expr, Expression);
	// todo 解析 Operand、MethodExpr。目前，只解析Operand。
	// 区分Operand和MethodExpr。
	// 2:点号连接起来的表达式；1：MethodExpr；0：Oprand。
	unsigned char type = 0;
	StartPeekToken();
	if(isTypeKeyWord(current_token.kind) == 1 || IsDataType(current_token.value.value_str) == 1){
		ParseType();
		// TODO 其实可以不必这么写。这样写，逻辑更清晰。
		if(current_token.kind == TK_LBRACE){
			type = 0;	
		}else if(current_token.kind == TK_LPARENTHESES){
			type = 3;
		}else if(current_token.kind == TK_DOT){
			type = 1;
		}
	}else{
		if(current_token.kind == TK_ID){
			NEXT_TOKEN;
			if(current_token.kind == TK_LPARENTHESES){
				NEXT_TOKEN;
				if(CurrentTokenIn(FIRST_Expression) == 1){
					ParseExpression();
					expect_comma;
					if(current_token.kind == TK_RPARENTHESES){
						type = 3;
					}else if(CurrentTokenIn(FIRST_Expression) == 1){
						type = 0;
					}
				}else{
					type = 0;
				}
			}else if(current_token.kind == TK_LBRACE){
				type = 0;
			}else if(current_token.kind == TK_DOT){
			//	type = 2;
			}
		}else if(current_token.kind == TK_LPARENTHESES){
			NEXT_TOKEN;
			if(CurrentTokenIn(FIRST_Expression) == 1){
//				"(" Expression ")"
				type = 0;
			}
		}else if(current_token.kind == TK_FUNC){
			NEXT_TOKEN;
			ParseParameters();
			ParseResult();		
			if(current_token.kind == TK_LBRACE){
				type = 0;
			}else{
				type = 1;
			}
		}else if(current_token.kind == TK_STRING){
			// do nothing
		}else if(current_token.kind == TK_ELLIPSIS){
			// do nothing
		}else if(current_token.kind == TK_NUM){
			type = 0;
		}
	}

	EndPeekToken();

	if(type == 1){
		expr = ParseMethodExpr();
	}else if(type == 0){
		expr = ParseOperand();
	}else if(type == 2){
		expr = ParseDotExpr();
	}else if(type == 3){
		expr = ParseConversion();
	}

	// TODO 不理解这里的逻辑，参考ucc的ParsePostfixExpression函数写的。
	while(1){
		AstExpression p;
		CREATE_AST_NODE(p, Expression);
		p->kids[0] = expr;

		switch(current_token.kind){
			case TK_DOT:
				ParseSelectorTypeAssertion(p);
				expr = p;
				break; 
			case TK_LPARENTHESES:
				p->op = OP_CALL;
				p->kids[1] = ParseArguments();
				expr = p;
				break; 
			case TK_LBRACKET:
				{
					AstNode node = ParseIndexSlice();
					if(node->kind == NK_SliceMeta){
						p->op = OP_SLICE;
					}else{
						p->op = OP_INDEX;
					}
					p->kids[1] = (AstExpression)node;
					expr = p;
				}
				break;
			default:
				return expr;
		}		
	}
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
		char *str = (char *)current_token.value.value_str;	
		if(IsTypeName(str) == 1) goto literal;
		expr = ParseOperandName();
//		if(IsSwitch() == 1){
//			expr = ParseOperandName();
//		}else{
//			StartPeekToken();
//			NEXT_TOKEN;
//			if(current_token.kind == TK_LBRACE){
//				EndPeekToken();
//				goto literal;
//			}
//			EndPeekToken();
//			expr = ParseOperandName();
//		}
	}else if(current_token.kind == TK_LPARENTHESES ){
		// TODO 需要给这种expr设置独立的op吗？
		// 不知道。搁置吧。
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
//	// todo 应该使用MALLOC分配内存空间才更妥当吗？
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
//	// todo 应该使用MALLOC分配内存空间才更妥当吗？
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	if(current_token.kind == TK_NUM){
		expr->ty = T(INT);
		expr->op = OP_CONST;
		union value v = {current_token.value.value_num,0};
		expr->val = v;
		NEXT_TOKEN;
	}else if(current_token.kind == TK_STRING){
		// TODO 可能不正确。临时这样做。
		expr->op = OP_STR;

		// TODO 在这里消耗了比较多时间。
		// value究竟是使用malloc分配内存还是使用局部变量分配内存？
		String value = (String)MALLOC(sizeof(struct string));
		int str_length = current_token.value.value_num;
		// String 的p成员一定需要分配内存。
		value->str = (char *)MALLOC(sizeof(char) * str_length);;
		value->length = str_length;
		strncpy(value->str, current_token.value.value_str, str_length);
		expr->val.p = value;

		expr->ty = ArrayOf(T(BYTE), str_length); 

		NEXT_TOKEN;
	}

	return expr;
}

int IsSwitch(){
	int result = 0;
	StartPeekToken();
	EXPECT(TK_ID);
	if(current_token.kind == TK_LBRACE){
		NEXT_TOKEN;
		if(current_token.kind == TK_CASE){
			result = 1;
		}
	}

	EndPeekToken();

	return result;
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
		if(IsTypeName(current_token.value.value_str)){
			StartPeekToken();
			EXPECT(TK_ID);
			if(current_token.kind == TK_LBRACE || current_token.kind == TK_LBRACKET){
				EndPeekToken();
				return 1;
			}

			EndPeekToken();
		}

		return 0;
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
		expr->kids[0] = ParseFunctionLit();
		expr->op = OP_FUNC_LIT;
	}else if(IsCompositeLit() == 1){
		expr->kids[0] = ParseCompositeLit(); 
		expr->op = OP_COMPOSITELIT;
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
		type = 1;
		if(i == 1 && current_token.kind == TK_DOT){
			type = 2;
			NEXT_TOKEN;
			if(current_token.kind == TK_LPARENTHESES){
				type = 1;
			}
			break;
		}
		NEXT_TOKEN;
	}
	EndPeekToken();

 	AstExpression expr;
 	CREATE_AST_NODE(expr, Expression);

	type = 1;
	if(type == 1){
		AstExpression expr1 = ParseIdentifier();
		if(expr == NULL){
			// TODO decl为空时，expr不是有意义的数据，直接返回可以吗？但我不知道应该怎么做。 			
			return expr;
		}
 		expr->op = OP_ID;
 		expr->val.p = (void *)MALLOC(sizeof(char) * MAX_NAME_LEN);
		strcpy((char *)(expr->val.p), expr1->val.p);
		return expr;
	}else if(type == 2){
		expr->kids[0] = ParseQualifiedIdent();
		expr->op = OP_PACKAGE;
	}else{
		ERROR("ParseOperandName错误\n", "");
	}

	return expr;
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
	fdecl->result = result;

	AstFunctionLit functionLit;
	CREATE_AST_NODE(functionLit, FunctionLit);

	functionLit->fdecl = fdecl;
	functionLit->body = body;

	return functionLit;
}

/**
 * CompositeLit  = LiteralType LiteralValue .
 */
AstCompositeLit ParseCompositeLit(){
	AstCompositeLit node;
	CREATE_AST_NODE(node, CompositeLit);
	
	node->literalType = ParseLiteralType();
	node->literalValue = ParseLiteralValue();	
 
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
// 创建层层分解的函数，代码变得简洁，复杂度降低，可是建立AST时可能会有许多不方便。怎么办？
AstLiteralValue ParseLiteralValue(){
	EXPECT(TK_LBRACE);

	AstLiteralValue value;
	CREATE_AST_NODE(value, LiteralValue);
	int hasKey = 1;

	AstKeyedElement header;
	CREATE_AST_NODE(header, KeyedElement);
	// header->next = element;
	AstKeyedElement *element = &(header->next);

	// TODO 不喜欢这样写。解析此段代码还用到了外部条件。
	// while(current_token.kind != TK_COMMA || current_token.kind != TK_RBRACE){
	while(current_token.kind != TK_RBRACE){
		if(current_token.kind == TK_COMMA)	continue;
		*element = ParseKeyedElement();
		if((*element)->key == NULL)	hasKey = 0;
		// element = &(element->next);
		element = &((*element)->next);
		// 处理逗号
//		NEXT_TOKEN;
		expect_comma;
	}

	expect_comma;
	EXPECT(TK_RBRACE);

	// TODO LiteralValue 不允许同时存在key:val和val两种模式。我目前没有对此进行检查。
	// TODO 补充上面的注释，数组的LiteralValue。

	value->hasKey = hasKey;
	value->keyedElement = header->next;

	return value;
}

/**
 * KeyedElement  = [ Key ":" ] Element .
Key           = FieldName | Expression | LiteralValue .
FieldName     = identifier .
Element       = Expression | LiteralValue .
 */
AstKeyedElement ParseKeyedElement(){

	AstKeyedElement elementNode;
	CREATE_AST_NODE(elementNode, KeyedElement);

	AstKey key;
	CREATE_AST_NODE(key, Key);

	AstElement element;
	CREATE_AST_NODE(element, Element);

	// element->key = key;

	// 这是唯一的难点。
	unsigned char flag = 0;
		if(current_token.kind == TK_LBRACE){
			key->lbrace = 1;
			key->literalValue = (AstNode)ParseLiteralValue();
		}else{
			key->lbrace = 0;
			if(CurrentTokenIn(FIRST_Expression) == 1){
				key->literalValue = (AstNode)ParseExpression();
			}else{	// FieldName
				key->expr = (AstNode)ParseIdentifier();
			}	
		}

		// 处理:
		if(current_token.kind == TK_COLON){
			flag = 1;
			EXPECT(TK_COLON);
		}

	// 只有值没有键，
	if(flag == 0){
		elementNode->key = NULL;
		key->kind = NK_Element;
		elementNode->element = (AstElement)key;
		return elementNode;
	}

	if(current_token.kind == TK_LBRACE){
		element->lbrace = 1;
		element->literalValue = (AstNode)ParseLiteralValue();
	}else{
		element->lbrace = 0;
		element->expr = (AstNode)ParseExpression();
	}

	elementNode->key = key;
	elementNode->element = element;

	return elementNode;
}


/**
 * MethodExpr    = ReceiverType "." MethodName .
ReceiverType  = Type .
MethodName         = identifier .
 */
AstExpression ParseMethodExpr(){
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	// TODO 想不到更好的op名称。
	expr->op = OP_METHOD;
	expr->kids[0] = (AstExpression)ParseType();
	AstExpression method = ParseIdentifier();
	if(method != NULL){	
		expr->val.p = method->val.p;
	}

	return expr;
}

// TODO 这个函数很复杂，工作量非常大，后期再处理。

// AstExpression ParseOperandName(){
// 	//dump_token_number();	
// 	AstExpression expr;
// 	CREATE_AST_NODE(expr, Expression);
// 	if(current_token.kind == TK_ID){
// 		// TODO 这是不正确的。临时这样做。
// 		expr->op = OP_NONE;
// 		expr->val.p = (void *)MALLOC(sizeof(char) * MAX_NAME_LEN);
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
//
//
AstExpression ParseCall()
{
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	expr->kids[0] = ParseOperandName();
	EXPECT(TK_LPARENTHESES);
	expr->kids[1] = ParseExpressionList();
	EXPECT(TK_RPARENTHESES);

	return expr;
}
