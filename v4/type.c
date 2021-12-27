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
		node = (AstNode)ParseTypeName();
		return node;
	}else if(isTypeKeyWord(kind) == 1){
		ParseTypeLit();
	}else{
		// todo 处理QualifiedIdent，暂时不实现
		printf("hi");
	}
}

// todo 不完善，只处理了TK_ID，没有处理TK_INT等。
AstTypedefName ParseTypeName(){
//	expect_token(TK_ID);
	
	AstTypedefName tname;
	CREATE_AST_NODE(tname, TypedefName);
	tname->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
	strcpy(tname->id, current_token.value.value_str);
	// todo 是否应该放在这里？
	NEXT_TOKEN;
	
	return tname;
}

AstNode ParseTypeLit(){
 	TokenKind kind = current_token.kind;
	// TODO 第一个条件判断不应该存在。
	if(kind == TK_CHAN){
		kind == TK_CHAN;
	}else if(kind == TK_MUL) {
		kind = TK_POINTER;
	}else if(kind == TK_STRUCT){
			kind = TK_STRUCT;
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
	}
	// return (TypeListParsers[kind]());
	return (TypeListParsers[kind - TK_MAP]());
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
// todo 暂时不实现这个函数。它属于其他元素的组成部分。
AstNode ParseQualifiedIdent(){

}

/**
 * ArrayType   = "[" ArrayLength "]" ElementType .
ArrayLength = Expression .
ElementType = Type .
 */
// TODO 没有建立AST
AstNode ParseArrayType(){
	printf("parse array\n");
	
	AstNode node;
	CREATE_AST_NODE(node, Node);

	expect_token(TK_LBRACKET);	
	ParseExpression();
	expect_token(TK_RBRACKET);	
	ParseType();

	return node;
}

/**
 * StructType    = "struct" "{" { FieldDecl ";" } "}" .
FieldDecl     = (IdentifierList Type | EmbeddedField) [ Tag ] .
EmbeddedField = [ "*" ] TypeName .
Tag           = string_lit .
 */
AstNode ParseStructType(){
	AstNode node;
	CREATE_AST_NODE(node, Node);

	expect_token(TK_STRUCT);
	expect_token(TK_LBRACE);	
	while(current_token.kind != TK_RBRACE){
		NO_TOKEN;
		ParseFieldDecl();
		// 处理;
		expect_semicolon;	
	}
	expect_token(TK_RBRACE);	

	return node;
}

/**
 * PointerType = "*" BaseType .
BaseType    = Type .
 */
AstNode ParsePointerType(){
	expect_token(TK_MUL);
	ParseType();	
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
AstNode ParseInterfaceType(){
	expect_token(TK_INTERFACE);
	expect_token(TK_LBRACE);
	while(current_token.kind != TK_RBRACE){
		NO_TOKEN;
		// todo 如何区分MethodSpec和InterfaceTypeName？
		// todo 没有想到区分二者的方法，暂时只解析MethodSpec。
		ParseMethodSpec();
		expect_semicolon;	
	}
	expect_token(TK_RBRACE);
}

AstNode ParseMethodSpec(){
	ParseMethodName();
	ParseSignature();
}

AstNode ParseInterfaceTypeName(){
	ParseTypeName();
}

AstNode ParseMethodName(){
	ParseIdentifier();
}

/**
 * SliceType = "[" "]" ElementType .
 */
AstNode ParseSliceType(){
	expect_token(TK_LBRACKET);	
	expect_token(TK_RBRACKET);	
	ParseType();
}

/**
 * MapType     = "map" "[" KeyType "]" ElementType .
KeyType     = Type .
 */
AstNode ParseMapType(){
	expect_token(TK_MAP);	
	expect_token(TK_LBRACKET);	
	ParseType();
	expect_token(TK_RBRACKET);	
	ParseType();
}

/**
 * ChannelType = ( "chan" | "chan" "<-" | "<-" "chan" ) ElementType .
 */
// todo get_token不能读取chan<-这类token，因此kind == TK_CHAN_RECEIVE这种情况无法处理。
AstNode ParseChannelType(){
	TokenKind kind = current_token.kind;
	if(kind == TK_CHAN){
		expect_token(TK_CHAN);
//	}else if(kind == TK_CHAN_SEND){
//		expect_token(TK_CHAN_SEND);
//	}else if(kind == TK_CHAN_RECEIVE){
//		expect_token(TK_CHAN_RECEIVE);
	}else{
		ERROR("expect a chan\n");
	}
	ParseType();
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
			node = ParseTypeName();
			break;
		case	ELEMENT:
			node = ParseType();
			break;
		default:
			// TODO 暂时什么也不做。
			// 神奇！default的break也不能缺少。
			break;
	}
 
	return node;
}
