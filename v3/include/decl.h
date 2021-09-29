#ifndef DECL_H
#define DECL_H

#include "lex.h"
#include "stmt.h"
#include "expr.h"


AstNodePtr direct_declarator();

AstNodePtr postfix_declarator();

AstNodePtr declarator();

AstNodePtr declaration();

void visit_declaration(AstNodePtr pNode);











#endif
