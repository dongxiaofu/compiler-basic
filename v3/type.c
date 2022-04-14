#include "ast.h"
#include "type2.h"
#include "decl.h"
#include "expr.h"


int isTypeKeyWord(TokenKind kind){


}

/**
 * Type      = TypeName | TypeLit | "(" Type ")" .
TypeName  = identifier | QualifiedIdent .
TypeLit   = ArrayType | StructType | PointerType | FunctionType | InterfaceType |
	    SliceType | MapType | ChannelType .
 */
AstNode ParseType(){
	TokenKind kind = current_token.kind;
	if(kind == TK_LPARENTHESES){
		expect_token(TK_LPARENTHESES);
		ParseType();
		expect_token(TK_RPARENTHESES);
	}else if(kind == TK_ID){ 
		ParseTypeName();
	}else if(isTypeKeyWord(kind) == 1){
		ParseTypeLit();
	}else{
		// todo 处理QualifiedIdent，暂时不实现
	}
}

AstNode ParseTypeName(){
	expect_token(TK_ID);
}

AstNode ParseTypeLit(){
 	TokenKind kind = current_token.kind;
	if(kind == TK_CHAIN_SEND || kind == TK_CHAIN_RECEIVE){
		kind == TK_CHAIN;
	}else if(kind == TK_NUL) {
		kind = TK_POINTER;
	}else if(kind == TK_LBRACKET){
		NEXT_TOKEN;
		if(kind == TK_RBRACKET){
			kind == TK_SLICE;
		}else{
			kind = TK_ARRAY;
		}
		// todo 退回处理了的token，交给解析函数去处理。
	}
	return (*TypeListParsers[kind]());
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

}

/**
 * StructType    = "struct" "{" { FieldDecl ";" } "}" .
FieldDecl     = (IdentifierList Type | EmbeddedField) [ Tag ] .
EmbeddedField = [ "*" ] TypeName .
Tag           = string_lit .
 */
AstNode ParseStructType(){

}

/**
 * PointerType = "*" BaseType .
BaseType    = Type .
 */
AstNode ParsePointerType(){

}

/**
 * FunctionType   = "func" Signature .
Signature      = Parameters [ Result ] .
Result         = Parameters | Type .
Parameters     = "(" [ ParameterList [ "," ] ] ")" .
ParameterList  = ParameterDecl { "," ParameterDecl } .
ParameterDecl  = [ IdentifierList ] [ "..." ] Type .
 */
AstNode ParseFunctionType(){

}

/**
 * InterfaceType      = "interface" "{" { ( MethodSpec | InterfaceTypeName ) ";" } "}" .
MethodSpec         = MethodName Signature .
MethodName         = identifier .
InterfaceTypeName  = TypeName .
 */
AstNode ParseInterfaceType(){

}

/**
 * SliceType = "[" "]" ElementType .
 */
AstNode ParseSliceType(){

}

/**
 * MapType     = "map" "[" KeyType "]" ElementType .
KeyType     = Type .
 */
AstNode ParseMapType(){

}

/**
 * ChannelType = ( "chan" | "chan" "<-" | "<-" "chan" ) ElementType .
 */
AstNode ParseChannelType(){

}
