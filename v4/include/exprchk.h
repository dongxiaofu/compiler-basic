#ifndef EXPRCHK_H
#define EXPRCHK_H

#include "ast.h"
#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"


void AssignInterfaceVariable(VariableSymbol sym, InitData idata);

AstExpression Constant(Type ty, union value val);
AstExpression ScalePointerOffset(AstExpression offset, int scale);

AstExpression CheckPrimaryExpression(AstExpression expr);
AstExpression CheckAssignmentExpression(AstExpression expr);

AstExpression CheckMemberAccess(AstExpression expr);
AstExpression CheckTypeAssert(AstExpression expr);
AstExpression CheckFunctionCall(AstExpression expr);

static AstExpression CheckPostfixExpression(AstExpression expr);

// TODO 占位符，暂时无作用。
static AstExpression CheckNOPExpression(AstExpression expr);

AstExpression CheckExpression(AstExpression expr);
// TODO 暂时放在这里。
AstExpression CheckExpressionList(AstExpression expr);
AstExpression CheckIdentifierList(AstExpression expr);
AstExpression CheckDeferExpr(AstExpression expr);

#endif
