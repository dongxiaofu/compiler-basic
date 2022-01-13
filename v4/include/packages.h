#ifndef PACKAGES_H
#define PACKAGES_H

#include <assert.h>


AstNode ParsePackageClause();
Token *ParseImportPath();
AstImportSpec ParseImportSpec();
AstImportDeclaration ParseImportDecl();
AstNode ParseSourceFile();
// AstNode Parse();
// AstNode Parse();
// AstNode Parse();
// AstNode Parse();

#endif
