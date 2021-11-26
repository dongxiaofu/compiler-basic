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
			return 1;	
		}else{
			
		}
			
	}

	// todo 处理指针。这种个性化处理方式太
	// 丑陋了，寻机优化。
	if(kind == TK_MUL) return 1;

	EndPeekToken();
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
	if(kind == TK_CHAN_SEND || kind == TK_CHAN_RECEIVE){
		kind == TK_CHAN;
	}else if(kind == TK_MUL) {
		kind = TK_POINTER;
	}else if(kind == TK_LBRACKET){
		StartPeekToken();
		NEXT_TOKEN;
		kind = current_token.kind;
		 EndPeekToken();
		if(kind == TK_RBRACKET){
			// todo 耗时两三个小时才找出这个低级错误。
			// kind == TK_SLICE;
			kind = TK_SLICE;
		}else{
			kind = TK_ARRAY;
		}
		EndPeekToken();
		// todo 退回处理了的token，交给解析函数去处理。
	}
	return (TypeListParsers[kind]());
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
AstNode ParseArrayType(){
	printf("parse array\n");
	
	expect_token(TK_LBRACKET);	
	ParseExpression();
	expect_token(TK_RBRACKET);	
	ParseType();
}

/**
 * StructType    = "struct" "{" { FieldDecl ";" } "}" .
FieldDecl     = (IdentifierList Type | EmbeddedField) [ Tag ] .
EmbeddedField = [ "*" ] TypeName .
Tag           = string_lit .
 */
AstNode ParseStructType(){
	expect_token(TK_STRUCT);
	expect_token(TK_LBRACE);	
	while(current_token.kind != TK_RBRACE){
		ParseFieldDecl();
		// 处理;
		expect_semicolon;	
	}
	expect_token(TK_RBRACE);	
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
	}else if(kind == TK_CHAN_SEND){
		expect_token(TK_CHAN_SEND);
	}else if(kind == TK_CHAN_RECEIVE){
		expect_token(TK_CHAN_RECEIVE);
	}else{
		ERROR("expect a chan\n");
	}
	ParseType();
}
