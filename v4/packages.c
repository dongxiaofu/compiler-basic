#include "lex.h"
#include "ast.h"
#include "decl.h"
#include "expr.h"
#include "packages.h"

/**
 * "package" PackageName .
 */
AstPackageClause ParsePackageClause(){
	AstPackageClause packageClause;
	CREATE_AST_NODE(packageClause, PackageClause);

	EXPECT(TK_PACKAGE);
	AstExpression identifier = ParseIdentifier();
	packageClause->packageName = identifier;

	return packageClause;
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
// AstSourceFile ParseSourceFile(){
AstTranslationUnit ParseSourceFile(){

	AstTranslationUnit sourceFile;
	CREATE_AST_NODE(sourceFile, TranslationUnit);
	sourceFile->importDecls = NULL;
	sourceFile->decls = NULL;

	sourceFile->packageClause = ParsePackageClause();
	expect_semicolon;
	
	AstImportDeclaration currentImportDecl = NULL;
	while(current_token.kind == TK_IMPORT){
		AstImportDeclaration next = ParseImportDecl();
		if(sourceFile->importDecls == NULL){
			currentImportDecl = next;
			sourceFile->importDecls = currentImportDecl;
		}else{
			currentImportDecl->next = currentImportDecl;
			currentImportDecl = next;
		}
		expect_semicolon;
	}

	AstDeclaration currentDecl = NULL;
	while(CurrentTokenIn(FIRST_Declaration) == 1){
		AstDeclaration next = declaration();
		if(sourceFile->decls == NULL){
			currentDecl = next;
			sourceFile->decls = currentDecl;
		}else{
			currentDecl->next = next;
			currentDecl = next;
		}
		expect_semicolon;
	}

	return sourceFile;
}
