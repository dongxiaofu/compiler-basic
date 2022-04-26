#ifndef STMTCHK_H
#define STMTCHK_H

#include "ast.h"
#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"

// TODO 暂时放在这里。
AstExpression CheckExpression(AstExpression expr);
AstExpression CheckExpressionList(AstExpression expr);
AstExpression CheckIdentifierList(AstExpression expr);

AstStatement CheckSimpleStmt(AstStatement stmt);
AstStatement CheckExprSwitchStmt(AstStatement stmt);
AstStatement CheckSelectCaseStatement(AstStatement stmt);
AstStatement CheckIfStatement(AstStatement stmt);
AstStatement CheckSwitchStatement(AstStatement stmt);
AstStatement CheckForStmt(AstStatement stmt);
AstStatement CheckGotoStatement(AstStatement stmt);
AstStatement CheckBreakStatement(AstStatement stmt);
AstStatement CheckContinueStatement(AstStatement stmt);
AstStatement CheckReturnStatement(AstStatement stmt);
AstStatement CheckCompoundStatement(AstStatement stmt);
AstStatement CheckIncDecStmt(AstStatement stmt);
AstStatement CheckLabelStmt(AstStatement stmt);
AstStatement CheckDeferStmt(AstStatement stmt);
AstStatement CheckFallthroughStmt(AstStatement stmt);
AstStatement CheckSelectStmt(AstStatement stmt);
AstStatement CheckGoStmt(AstStatement stmt);
AstStatement CheckAssignmentsStmt(AstStatement stmt);

AstStatement CheckStatement(AstStatement stmt);

#endif
