#ifndef DUMP_IR_H
#define DUMP_IR_H

void DumpFunctionCall(IRInst irinst);
void DumpIR(IRInst irinst);
void DumpBBlock(BBlock bblock);
void DumpFunction(FunctionSymbol fsym);
void DumpTranslateUnit(AstTranslationUnit transUnit);

#endif
