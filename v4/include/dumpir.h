#ifndef DUMP_IR_H
#define DUMP_IR_H

void DumpIR(IRInst irinst);
void DumpBBlock(BBlock bblock);
void DumpFunction(FunctionSymbol fsym);
void DumpTranslateUnit(AstTranslationUnit transUnit);

#endif
