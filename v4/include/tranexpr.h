#ifndef TRAN_EXPR_H
#define TRAN_EXPR_H

#include <assert.h>
#include "gen.h"

Symbol Addressof(Symbol sym);
Symbol Deref(Type ty, Symbol addr);
Symbol Offset(Type ty, Symbol addr, int voff, int coff);

Symbol TranslateFunctionCall(AstExpression expr);
Symbol TranslateAssignmentExpression(AstExpression expr);
Symbol TranslateArrayIndex(AstExpression expr);
AstExpression Not(AstExpression expr);
void TranslateBranch(AstExpression expr, BBlock trueBB, BBlock falseBB);
Symbol TranslateBranchExpression(AstExpression expr);
Symbol TranslateBinaryExpression(AstExpression expr);
Symbol TranslatePostfixExpression(AstExpression expr);
Symbol TranslateUnaryExpression(AstExpression expr);
Symbol TranslatePrimaryExpression(AstExpression expr);
Symbol TranslateCastExpression(AstExpression expr);
Symbol TranslateIncDecExpression(AstExpression expr);
Symbol TranslateMemberAccess(AstExpression expr);
Symbol TranslateExpression(AstExpression expr);
#endif
