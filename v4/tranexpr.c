#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "exprchk.h"
#include "tranexpr.h"

Symbol TranPrimaryExpression(AstExpression expr)
{

	return expr->val.p;
}
