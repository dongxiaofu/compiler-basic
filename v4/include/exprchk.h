#ifndef EXPRCHK_H
#define EXPRCHK_H

#include "ast.h"
#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"

AstExpression CheckAssignmentExpression(AstExpression expr);
static AstExpression CheckPostfixExpression(AstExpression expr);

// TODO 占位符，暂时无作用。
static AstExpression CheckNOPExpression(AstExpression expr);

AstExpression CheckExpression(AstExpression expr);
// TODO 暂时放在这里。
AstExpression CheckExpressionList(AstExpression expr);
AstExpression CheckIdentifierList(AstExpression expr);
AstExpression CheckDeferExpr(AstExpression expr);

#endif
