#ifndef DECLCHK_H
#define DECLCHK_H

// #include "grammer.h"
#include "ast.h"
#include "lex.h"
// #include "grammer.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"
// #include "grammer.h"

void EndRecord(RecordType rty);
Field AddField(RecordType rty, char *id, Type ty);
RecordType StartRecord();

RecordType CheckStructSpecifier(AstStructSpecifier specs);
MapType CheckMapSpecifier(AstMapSpecifier specs);
ArrayType CheckArraySpecifier(AstArrayTypeSpecifier specs);
void CheckDeclarationSpecifiers(AstSpecifiers specs);

AstKeyedElement LookupElement(AstKeyedElement element, char *name);
CanAssign(Type ty, AstExpression val);

InitData CheckMapInitializer(AstCompositeLit compositeLit);
InitData CheckStructInitializer(AstCompositeLit compositeLit);
InitData CheckArrayInitializer(AstCompositeLit compositeLit);
InitData CheckCompositeLitInitializer(AstCompositeLit compositeLit);
void CheckInitializer(AstInitializer init);

void CheckDeclaration(AstDeclaration decls);
void CheckLocalDeclaration(AstDeclaration decls);
int CheckBlock(AstBlock block);
void CheckFunction(AstFunction p);
void CheckTranslationUnit(AstTranslationUnit transUnit);
void CheckGlobalDeclaration(AstDeclaration decl);

#endif
