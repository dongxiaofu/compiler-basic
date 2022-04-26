#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "stmtchk.h"

AstStatement (*StmtCheckers[])(AstStatement) = {
	CheckCaseStatement,
	CheckIfStatement,
	CheckSwitchStatement,
	CheckForStmt,
	CheckGotoStatement,
	CheckBreakStatement,
	CheckContinueStatement,
	CheckReturnStatement,
	CheckCompoundStatement,
	CheckIncDecStmt,
	CheckLabelStmt,
	CheckDeferStmt,
	CheckFallthroughStmt,
	CheckSelectStmt,
	CheckGoStmt,
	CheckAssignmentsStmt
};

AstStatement CheckCaseStatement(AstStatement stmt)
{

}

AstStatement CheckIfStatement(AstStatement stmt)
{

}

AstStatement CheckSwitchStatement(AstStatement stmt)
{

}

AstStatement CheckForStmt(AstStatement stmt)
{

}

AstStatement CheckGotoStatement(AstStatement stmt)
{

}

AstStatement CheckBreakStatement(AstStatement stmt)
{

}

AstStatement CheckContinueStatement(AstStatement stmt)
{

}

AstStatement CheckReturnStatement(AstStatement stmt)
{

}

AstStatement CheckCompoundStatement(AstStatement stmt)
{

}

AstStatement CheckIncDecStmt(AstStatement stmt)
{

}

AstStatement CheckLabelStmt(AstStatement stmt)
{

}

AstStatement CheckDeferStmt(AstStatement stmt)
{

}

AstStatement CheckFallthroughStmt(AstStatement stmt)
{

}

AstStatement CheckSelectStmt(AstStatement stmt)
{

}

AstStatement CheckGoStmt(AstStatement stmt)
{

}

AstStatement CheckAssignmentsStmt(AstStatement stmt)
{

}
