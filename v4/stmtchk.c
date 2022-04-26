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

AstStatement CheckStatement(AstStatement stmt)
{
	return (*StmtCheckers[stmt->kind - NK_CaseStatement])(stmt);
}

AstStatement CheckCaseStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckIfStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckSwitchStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckForStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckGotoStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckBreakStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckContinueStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckReturnStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckCompoundStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckIncDecStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckLabelStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckDeferStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckFallthroughStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckSelectStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckGoStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckAssignmentsStmt(AstStatement stmt)
{

return stmt;
}
