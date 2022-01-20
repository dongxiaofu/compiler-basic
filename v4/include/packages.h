#ifndef PACKAGES_H
#define PACKAGES_H

#include <assert.h>

AstPackageClause ParsePackageClause();
Token *ParseImportPath();
AstImportSpec ParseImportSpec();
AstImportDeclaration ParseImportDecl();
AstSourceFile ParseSourceFile();

#endif
