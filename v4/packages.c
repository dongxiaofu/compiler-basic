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
Token *ParseImportPath(){
	if(current_token.kind != TK_STRING){
		perror("expect TK_STRING");
		exit(-4);
	}

	Token *token = (Token *)malloc(sizeof(Token));
	memcpy(token, &current_token, sizeof(Token));

	NEXT_TOKEN;
	
	return token;
}

/**
 * [ "." | PackageName ] ImportPath .
 * ImportPath       = string_lit .
 * import   "lib/math"         math.Sin
 * import m "lib/math"         m.Sin
 * import . "lib/math"         Sin
 */
AstImportSpec ParseImportSpec(){
	AstImportSpec importSpec;
	CREATE_AST_NODE(importSpec, ImportSpec);

	AstDeclarator packageName;
	CREATE_AST_NODE(packageName, Declarator);
	packageName->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
	if(current_token.kind == TK_DOT){
		EXPECT(TK_DOT);
		strcpy(packageName->id, current_token.value.value_str);
	}

	if(current_token.kind == TK_ID){
		EXPECT(TK_ID);
		strcpy(packageName->id, current_token.value.value_str);
	}

	Token *token = ParseImportPath();
	AstDeclarator importPath;
	CREATE_AST_NODE(importPath, Declarator);
	importPath->id = (char *)malloc(sizeof(char) * MAX_NAME_LEN);
	strcpy(importPath->id, token->value.value_str);
	
	return importSpec;
}

/**
 * ImportDecl       = "import" ( ImportSpec | "(" { ImportSpec ";" } ")" ) .
 */
AstImportDeclaration ParseImportDecl(){
	EXPECT(TK_IMPORT);	
	AstImportDeclaration decl;
	CREATE_AST_NODE(decl, ImportDeclaration);
	decl->importSpec = NULL;

	if(current_token.kind != TK_LPARENTHESES){
		AstImportSpec importSpec = ParseImportSpec();
		decl->importSpec = importSpec;
		return decl;
	} 

//	EXPECT(TK_LPARENTHESES);	
//	ParseImportSpec();
//	while(current_token.kind == TK_SEMICOLON){
//		NEXT_TOKEN;
//		ParseImportSpec();
//	}
//	EXPECT(TK_RPARENTHESES);	

	AstImportSpec importSpec;

	EXPECT(TK_LPARENTHESES);	
	while(current_token.kind != TK_RPARENTHESES){
		NO_TOKEN;
		AstImportSpec currentImportSpec = ParseImportSpec();
		if(decl->importSpec == NULL){
			importSpec = currentImportSpec;
			decl->importSpec = importSpec;
		}else{
			importSpec->next = currentImportSpec;
			importSpec = currentImportSpec;
		}

		expect_semicolon;
	}
	EXPECT(TK_RPARENTHESES);	

	return decl;
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
