#ifndef TRAN_STMT_H
#define TRAN_STMT_H
void TranslateBlock(AstBlock block);
void TranslateExprSwitchStmt(AstStatement stmt);
void TranslateTypeSwitchStmt(AstStatement stmt);
void TranslateSelectCaseStatement(AstStatement stmt);
void TranslateExpressionStmt(AstStatement stmt);
void TranslateShortVarDecl(AstStatement stmt);
void TranslateIfStatement(AstStatement stmt);
void TranslateSwitchStatement(AstStatement stmt);
void TranslateForStmt(AstStatement stmt);
void TranslateGotoStatement(AstStatement stmt);
void TranslateBreakStatement(AstStatement stmt);
void TranslateContinueStatement(AstStatement stmt);
void TranslateReturnStatement(AstStatement stmt);
void TranslateCompoundStatement(AstStatement stmt);
void TranslateIncDecStmt(AstStatement stmt);
void TranslateLabelStmt(AstStatement stmt);
void TranslateDeferStmt(AstStatement stmt);
void TranslateFallthroughStmt(AstStatement stmt);
void TranslateSelectStmt(AstStatement stmt);
void TranslateGoStmt(AstStatement stmt);
void TranslateSendStmt(AstStatement stmt);
void TranslateRecvStmt(AstStatement stmt);
void TranslateOneAssignmentsStmt(stmt);
void TranslateAssignmentsStmt(AstStatement stmt);

void PrintBBlock(BBlock bblock);
void TranslateFunction(AstFunction function);
void Translate(AstTranslationUnit transUnit);

#endif
