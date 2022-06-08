#ifndef TRAN_STMT_H
#define TRAN_STMT_H

Symbol TranslateExprSwitchStmt(AstStatement stmt);
Symbol TranslateTypeSwitchStmt(AstStatement stmt);
Symbol TranslateSelectCaseStatement(AstStatement stmt);
Symbol TranslateExpressionStmt(AstStatement stmt);
Symbol TranslateShortVarDecl(AstStatement stmt);
Symbol TranslateIfStatement(AstStatement stmt);
Symbol TranslateSwitchStatement(AstStatement stmt);
Symbol TranslateForStmt(AstStatement stmt);
Symbol TranslateGotoStatement(AstStatement stmt);
Symbol TranslateBreakStatement(AstStatement stmt);
Symbol TranslateContinueStatement(AstStatement stmt);
Symbol TranslateReturnStatement(AstStatement stmt);
Symbol TranslateCompoundStatement(AstStatement stmt);
Symbol TranslateIncDecStmt(AstStatement stmt);
Symbol TranslateLabelStmt(AstStatement stmt);
Symbol TranslateDeferStmt(AstStatement stmt);
Symbol TranslateFallthroughStmt(AstStatement stmt);
Symbol TranslateSelectStmt(AstStatement stmt);
Symbol TranslateGoStmt(AstStatement stmt);
Symbol TranslateSendStmt(AstStatement stmt);
Symbol TranslateRecvStmt(AstStatement stmt);
Symbol TranslateAssignmentsStmt(AstStatement stmt);
Symbol TranslateStatement(AstStatement stmt);

FunctionSymbol TranslateFunction(AstFunction function);
void Translate(AstTranslationUnit transUnit);

#endif
