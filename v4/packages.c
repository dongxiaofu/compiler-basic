#include "lex.h"
#include "ast.h"
#include "decl.h"
#include "expr.h"
#include "packages.h"

/**
 * "package" PackageName .
 */
AstNode ParsePackageClause(){
	EXPECT(TK_PACKAGE);
	ParseIdentifier();

	AstNode node;
	CREATE_AST_NODE(node, Node);
	return node;
}

/**
 * string_lit .
 */
AstNode ParseImportPath(){
	if(current_token.kind != TK_STRING){
		perror("expect TK_STRING");
		exit(-4);
	}

	NEXT_TOKEN;

	AstNode node;
	CREATE_AST_NODE(node, Node);
	return node;
}

/**
 * [ "." | PackageName ] ImportPath .
 */
AstNode ParseImportSpec(){
	if(current_token.kind == TK_DOT){
		EXPECT(TK_DOT);
	}

	if(current_token.kind == TK_ID){
		EXPECT(TK_ID);
	}

	ParseImportPath();

	AstNode node;
	CREATE_AST_NODE(node, Node);
	return node;
}

/**
 * ImportDecl       = "import" ( ImportSpec | "(" { ImportSpec ";" } ")" ) .
 */
AstNode ParseImportDecl(){
	EXPECT(TK_IMPORT);	
	AstNode node;
	CREATE_AST_NODE(node, Node);

	if(current_token.kind != TK_LPARENTHESES){
		ParseImportSpec();
		return node;
	} 

//	EXPECT(TK_LPARENTHESES);	
//	ParseImportSpec();
//	while(current_token.kind == TK_SEMICOLON){
//		NEXT_TOKEN;
//		ParseImportSpec();
//	}
//	EXPECT(TK_RPARENTHESES);	

	EXPECT(TK_LPARENTHESES);	
	while(current_token.kind != TK_RPARENTHESES){
		NO_TOKEN;
		ParseImportSpec();
		expect_semicolon;
		// NEXT_TOKEN;
	}
	EXPECT(TK_RPARENTHESES);	

	return node;
}

/**
 * SourceFile       = PackageClause ";" { ImportDecl ";" } { TopLevelDecl ";" } .
 */
AstNode ParseSourceFile(){
	ParsePackageClause();
	expect_semicolon;
	
	while(current_token.kind == TK_IMPORT){
		ParseImportDecl();
		expect_semicolon;
	}

	while(CurrentTokenIn(FIRST_Declaration) == 1){
		declaration();
		expect_semicolon;
	}

	AstNode node;
	CREATE_AST_NODE(node, Node);
	return node;
}
