#include "ast.h"
#include "lex.h"
#include "type2.h"
#include "decl.h"
#include "expr.h"

int isTypeKeyWord(TokenKind kind){
	StartPeekToken();
	if(kind == TK_LBRACKET){
		StartPeekToken();
		NEXT_TOKEN;
		if(current_token.kind != TK_RBRACKET) ParseExpression();
		if(current_token.kind == TK_RBRACKET){
			EndPeekToken();
			EndPeekToken();
			return 1;	
		}else{
			EndPeekToken();
		}
			
	}else if(kind == TK_RECEIVE){
			kind = TK_CHAN;
	}
			
	EndPeekToken();
	// todo 处理指针。这种个性化处理方式太
	// 丑陋了，寻机优化。
	if(kind == TK_MUL) return 1;

	// EndPeekToken();
	return (TK_FUNC <= kind && kind <= TK_ARRAY);
}

/**
 * Type      = TypeName | TypeLit | "(" Type ")" .
TypeName  = identifier | QualifiedIdent .
TypeLit   = ArrayType | StructType | PointerType | FunctionType | InterfaceType |
	    SliceType | MapType | ChannelType .
 */
AstNode ParseType(){
	// 跳过type关键字
//	NEXT_TOKEN;
	AstNode node;
	CREATE_AST_NODE(node, Node); 
	TokenKind kind = current_token.kind;
	if(kind == TK_LPARENTHESES){
		expect_token(TK_LPARENTHESES);
		ParseType();
		expect_token(TK_RPARENTHESES);
	}else if(kind == TK_ID){	// todo 不仅TK_ID应该在这里解析，TK_INT等golang内置数据类型也应该在这里解析。 
		node = (AstNode)ParseBasicType();
		return node;
	}else if(isTypeKeyWord(kind) == 1){
		return ParseTypeLit();
	}else{
		ERROR("ParseType 错误\n", "");
	}

	return node;
}

// todo 不完善，只处理了TK_ID，没有处理TK_INT等。
// TypeName  = identifier | QualifiedIdent .
// AstTypedefName ParseTypeName(){
// AstNode ParseTypeName(){
AstNode ParseBasicType(){
//	expect_token(TK_ID);
//	type：0--identifier，1--QualifiedIdent。
	unsigned char type = 0;
	StartPeekToken();
	ParseIdentifier();
	if(current_token.kind == TK_DOT){
		type = 1;
	}
	EndPeekToken();

	AstSpecifiers spec;
	CREATE_AST_NODE(spec, Specifiers);
	
	if(type == 0){	
		AstTypedefName tname;
		CREATE_AST_NODE(tname, TypedefName);
		tname->id = (char *)MALLOC(sizeof(char) * MAX_NAME_LEN);
		strcpy(tname->id, current_token.value.value_str);
		NEXT_TOKEN;
		spec->tySpecs = tname;	
	}else{
		AstExpression expr;
		CREATE_AST_NODE(expr, Expression);
		
		expr->op = OP_DOT;
		expr->kids[0] = ParseIdentifier();
		EXPECT(TK_DOT);
		expr->kids[1] = ParseIdentifier();
		spec->tySpecs = expr;
	}

	return spec;
}

/**
 * Type      = TypeName | TypeLit | "(" Type ")" .
TypeName  = identifier | QualifiedIdent .
TypeLit   = ArrayType | StructType | PointerType | FunctionType | InterfaceType |
	    SliceType | MapType | ChannelType .
 */
AstNode ParseTypeLit(){
 	TokenKind kind = current_token.kind;
//	enum LITERAL_TYPE type = GetTypeKind();	
	// TODO 第一个条件判断不应该存在。
	if(kind == TK_CHAN){
		kind == TK_CHAN;
	}else if(kind == TK_MUL) {
		kind = TK_POINTER;
	}else if(kind == TK_STRUCT){
			kind = TK_STRUCT;
	}else if(kind == TK_FUNC){
			kind = TK_FUNC;
	}else if(kind == TK_INTERFACE){
			kind = TK_INTERFACE;
	}else if(kind == TK_MAP){
			kind = TK_MAP;
	}else if(kind == TK_CHAN || kind == TK_RECEIVE){
			kind = TK_CHAN;
	}else if(kind == TK_LBRACKET){
		StartPeekToken();
		NEXT_TOKEN;
		kind = current_token.kind;
		EndPeekToken();
		if(kind == TK_RBRACKET){
			// todo 耗时两三个小时才找出这个低级错误。
			// kind == TK_SLICE;
			kind = TK_SLICE;
//			NEXT_TOKEN;
//			expect_token(TK_STRUCT);
		}else{
			kind = TK_ARRAY;
		}
		// EndPeekToken();
		// todo 退回处理了的token，交给解析函数去处理。
	}else{
	//	kind = TK_ID;
	}
	// return (TypeListParsers[kind]());
	return (TypeListParsers[kind - TK_FUNC]());
}

/**
 * todo 如此长的switch，应该使用函数指针数组
 * 实现。
 * 不拖延了，现在就重构。
 */
// AstNode ParseTypeLit(){
// 	TokenKind kind = current_token.kind;
// 	switch(kind){
// 		case TK_FUNC:
// 			ParseFunctionType();
// 			break;
// 		case TK_ARRAY:
// 			ParseArrayType();
// 			break;
// 		case TK_STRUCT:
// 			ParseStructType();
// 			break;
// 		case TK_POINTER:
// 			ParsePointerType();
// 			break;
// 		case TK_INTERFACE:
// 			ParseInterfaceType();
// 			break;
// 		case TK_SLICE:
// 			ParseSliceType();
// 			break;
// 		case TK_MAP:
// 			ParseMapType();
// 			break;
// 		case TK_CHANNEL:
// 			ParseChannelType();
// 			break;
// 		default:
// 			break;
// 
// 	}
// }
/**
 * QualifiedIdent = PackageName "." identifier .
 */
AstQualifiedIdent ParseQualifiedIdent(){
	AstQualifiedIdent ident;
	CREATE_AST_NODE(ident, QualifiedIdent);
	AstExpression expr1 = ParseIdentifier();	
	if(expr1 == NULL){
		ERROR("QualifiedIdent的PackageName不能是NULL\n", "");
	}
	ident->packageName = (char *)(expr1->val.p);
	EXPECT(TK_DOT);
	AstExpression expr2 = ParseIdentifier();	
	if(expr2 == NULL){
		ERROR("QualifiedIdent的identifier不能是NULL\n", "");
	}
	ident->identifier = (char *)(expr2->val.p);

	return ident;
}

/**
 * ArrayType   = "[" ArrayLength "]" ElementType .
ArrayLength = Expression .
ElementType = Type .
 */
AstArrayTypeSpecifier ParseArrayType(){
	printf("parse array\n");
	
	AstArrayTypeSpecifier spec;
	CREATE_AST_NODE(spec, ArrayTypeSpecifier);

	expect_token(TK_LBRACKET);	
	spec->expr = (AstExpression)ParseExpression();
	expect_token(TK_RBRACKET);	
	spec->type = (AstNode)ParseType();

	return spec;
}

/**
 * StructType    = "struct" "{" { FieldDecl ";" } "}" .
FieldDecl     = (IdentifierList Type | EmbeddedField) [ Tag ] .
EmbeddedField = [ "*" ] TypeName .
// Tag的作用是什么？
Tag           = string_lit .
 */
AstStructSpecifier ParseStructType(){
	AstStructDeclarator header;
	CREATE_AST_NODE(header, StructDeclarator);
	AstNode cur = NULL;

	expect_token(TK_STRUCT);
	expect_token(TK_LBRACE);	
	while(current_token.kind != TK_RBRACE){
		NO_TOKEN;
		FieldDecl fieldDecl = ParseFieldDecl();
		if(header->next == NULL){
			header->next = (AstNode)(fieldDecl->member);
			// cur = fieldDecl->tail;
		}else{
			cur->next = (AstNode)(fieldDecl->member);	
		}
		cur = fieldDecl->tail;
		// 处理;
		expect_semicolon;	
	}
	expect_token(TK_RBRACE);	

	AstStructSpecifier stSpec;
	CREATE_AST_NODE(stSpec, StructSpecifier);
	stSpec->stDecls = header->next;	

	return stSpec;
}

/**
 * PointerType = "*" BaseType .
BaseType    = Type .
 */
AstPointerDeclarator ParsePointerType(){
	AstPointerDeclarator ptrDecl;
	CREATE_AST_NODE(ptrDecl, PointerDeclarator);
	expect_token(TK_MUL);
	// TODO 真的很烦这种胡乱强制转换数据类型的做法。
	ptrDecl->dec = (AstDeclarator)ParseType();	

	return ptrDecl;
}

/**
 * FunctionType   = "func" Signature .
Signature      = Parameters [ Result ] .
Result         = Parameters | Type .
Parameters     = "(" [ ParameterList [ "," ] ] ")" .
ParameterList  = ParameterDecl { "," ParameterDecl } .
ParameterDecl  = [ IdentifierList ] [ "..." ] Type .
 */
// AstNode ParseFunctionType(){
//	expect_token(TK_FUNC);
//	ParseSignature();	
// }

// AstNode ParseSignature(){
// 	ParseParameters();
// 	ParseResult();
// }
// 
// AstNode ParseParameters(){
// 	expect_token(TK_LPARENTHESES);
// 	// todo 这样处理可选产生式是否可行？
// 	ParseParameterList();
// 	expect_semicolon;
// 	// TK_COMMA
// 	expect_token(TK_RPARENTHESES);
// }
// 
// AstNode ParseResult(){
// 	if(current_token.kind == TK_LPARENTHESES){
// 		ParseParameters();
// 	}else{
// 		ParseType();
// 	} 
// }
// 
// AstNode ParseParameterList(){
// 	ParseParameterDecl();
// 	while(current_token.kind == TK_COMMA){
// 		NEXT_TOKEN;
// 		ParseParameterDecl();
// 	}
// }
/**
 * InterfaceType      = "interface" "{" { ( MethodSpec | InterfaceTypeName ) ";" } "}" .
MethodSpec         = MethodName Signature .
MethodName         = identifier .
InterfaceTypeName  = TypeName .
 */
AstInterfaceDeclaration ParseInterfaceType(){
	expect_token(TK_INTERFACE);
	expect_token(TK_LBRACE);
	AstInterfaceDeclaration interfaceDeclaration;
	CREATE_AST_NODE(interfaceDeclaration, InterfaceDeclaration);

	AstNode header;
	CREATE_AST_NODE(header, Node);
	AstNode currentNode = NULL;

	while(current_token.kind != TK_RBRACE){
		NO_TOKEN;
		// todo 如何区分MethodSpec和InterfaceTypeName？
		// todo 没有想到区分二者的方法，暂时只解析MethodSpec。
		unsigned char type = 0;
		StartPeekToken();
		while(current_token.kind != TK_RBRACE){
			if(current_token.kind == TK_LPARENTHESES){
				type = 1;
				break;
			}
			NEXT_TOKEN;
		}
		EndPeekToken();
		AstNode node;
		if(type == 1){
			AstMethodSpec methodSpec = ParseMethodSpec();
			node = (AstNode)methodSpec;
		}else{
			node = ParseBasicType();
		}
		if(header->next == NULL){
			currentNode = node;
			header->next = currentNode;
		}else{
			currentNode->next = node;
			currentNode = node;
		}
		expect_semicolon;	
	}
	expect_token(TK_RBRACE);

	interfaceDeclaration->interfaceDecs = header->next;

	return interfaceDeclaration;	
}

/**
 * MethodSpec         = MethodName Signature .
   MethodName         = identifier .
 */
AstMethodSpec ParseMethodSpec(){
	AstMethodSpec methodSpec;
	CREATE_AST_NODE(methodSpec, MethodSpec);
	methodSpec->funcName = ParseMethodName();	

	AstParameterTypeList paramTypeList;
	CREATE_AST_NODE(paramTypeList, ParameterTypeList);
	AstParameterDeclaration params = ParseParameters(); 
	paramTypeList->paramDecls = params;
	methodSpec->paramTyList = paramTypeList; 
	
	AstParameterTypeList signature;
	CREATE_AST_NODE(signature, ParameterTypeList);
	AstParameterDeclaration result = ParseResult(); 
	signature->paramDecls = result;
	methodSpec->sig = signature;

	return methodSpec;
}

AstNode ParseInterfaceTypeName(){
	return ParseBasicType(); 
}

AstNode ParseMethodName(){
	return ParseFunctionName();
}

/**
 * SliceType = "[" "]" ElementType .
 */
AstSliceType ParseSliceType(){
	expect_token(TK_LBRACKET);	
	expect_token(TK_RBRACKET);	

	AstSliceType sliceType;
	CREATE_AST_NODE(sliceType, SliceType);
	sliceType->node = ParseType();

	return sliceType;
}

/**
 * MapType     = "map" "[" KeyType "]" ElementType .
KeyType     = Type .
 */
AstMapSpecifier ParseMapType(){
	AstMapSpecifier mapType;
	CREATE_AST_NODE(mapType, MapSpecifier);

	expect_token(TK_MAP);	
	expect_token(TK_LBRACKET);	
	AstNode KeyType = ParseType();
	expect_token(TK_RBRACKET);	
	AstNode elementType = ParseType();

	mapType->keyType = KeyType;
	mapType->elementType = elementType;

	return mapType;
}

/**
 * ChannelType = ( "chan" | "chan" "<-" | "<-" "chan" ) ElementType .
 * chan T          // can be used to send and receive values of type T
 * chan<- float64  // can only be used to send float64s
 * <-chan int      // can only be used to receive ints
 */
AstChannelType ParseChannelType(){
	AstChannelType astChannelType;
	CREATE_AST_NODE(astChannelType, ChannelType);
	TokenKind kind = current_token.kind;
	// ChannelType type;
	enum ChannelType type;
	if(kind == TK_CHAN){
		expect_token(TK_CHAN);
		type = CT_SendReceive;
		int is_chan = 0;
		StartPeekToken();
		if(current_token.kind == TK_RECEIVE){
			is_chan = 1;
		}	
		EndPeekToken();
		if(is_chan == 1){
			expect_token(TK_RECEIVE);
			type = CT_Send;
		}
	}else if(kind == TK_RECEIVE){
		expect_token(TK_RECEIVE);
		expect_token(TK_CHAN);
		type = CT_Receive;
	}else{
		ERROR("expect a chan\n", "");
	}

	astChannelType->type = type;
	astChannelType->elementType = ParseType();

	return astChannelType;
}

// 获取数据类型的种类，例如数组、结构体等。
// TODO 这只是一部分，还有其他数据类型的种类。
int GetTypeKind(){
	enum LITERAL_TYPE type;

	StartPeekToken();
	if(current_token.kind == TK_STRUCT){
		type = STRUCT;
	}else if(current_token.kind == TK_MAP){
		type = MAP;
	}else if(current_token.kind == TK_ID){
		type = NAME;
	}else if(current_token.kind == TK_LBRACKET){
		NEXT_TOKEN;
		if(current_token.kind == TK_RBRACKET){
			type = SLICE;
		}else if(current_token.kind == TK_ELLIPSIS){
			// TODO 需要判断第三个token吗？
			type = ELEMENT;
		}else{
			type = ARRAY;
		}
	}
	
//	else if(){
//
//	}else if(){
	EndPeekToken();

	return type;
}

/**
 * LiteralType   = StructType | ArrayType | "[" "..." "]" ElementType |
                SliceType | MapType | TypeName .
 */
AstNode ParseLiteralType(){
	AstNode node;
	CREATE_AST_NODE(node, Node);
	
	// TODO 能这样使用enum吗？
	enum LITERAL_TYPE type = GetTypeKind();	
	switch(type){
		case	ARRAY:
			node = ParseArrayType();
			break;
		case	SLICE:
			node = ParseSliceType();
			break;
		case	STRUCT:
			node = ParseStructType();
			break;
		case	MAP:
			node = ParseMapType();
			break;
		case	NAME:
			node = ParseBasicType();
			break;
		case	ELEMENT:
			node = ParseVariableElementType(); 
			break;
		default:
			// TODO 暂时什么也不做。
			// 神奇！default的break也不能缺少。
			node = ParseBasicType();
			break;
	}
 
	return node;
}

/**
 * FunctionType   = "func" Signature .
Signature      = Parameters [ Result ] .
Result         = Parameters | Type .
Parameters     = "(" [ ParameterList [ "," ] ] ")" .
ParameterList  = ParameterDecl { "," ParameterDecl } .
ParameterDecl  = [ IdentifierList ] [ "..." ] Type .
 */
AstFunctionDeclarator ParseFunctionType(){

	NEXT_TOKEN;

	AstFunctionDeclarator fdec;
	CREATE_AST_NODE(fdec, FunctionDeclarator);

	AstParameterTypeList paramTypeList;
	CREATE_AST_NODE(paramTypeList, ParameterTypeList);
	AstParameterDeclaration params = ParseParameters(); 
	paramTypeList->paramDecls = params;
	fdec->paramTyList = paramTypeList;
	
	AstParameterTypeList signature;
	CREATE_AST_NODE(signature, ParameterTypeList);
	AstParameterDeclaration result = ParseResult(); 
	signature->paramDecls = result;
	fdec->result = signature;
	
	return fdec;
}

// "[" "..." "]" ElementType
AstVariableElementType ParseVariableElementType(){
	AstVariableElementType node;

	CREATE_AST_NODE(node, VariableElementType);
	EXPECT(TK_LBRACKET);
	EXPECT(TK_ELLIPSIS);
	EXPECT(TK_RBRACKET);

	node->node = ParseType();

	return node;
}

void SetupTypeSystem(){
//	(Types + INT)->size = INT_SIZE;
//	T(INT)->size = 5;
 	T(BYTE)->size = BYTE_SIZE;	
 	T(INT)->size = INT_SIZE;
}
