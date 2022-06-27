#ifndef REG_H
#define REG_H

#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "ast.h"
#include "expr.h"

enum {EAX, EBX, ECX, EDX, EBP, ESP, EDI, ESI};

#define NO_REG -1

Symbol CreateReg(char *name, char *iname, int no);
void SpillReg(Symbol reg);
void ClearRegs();
int FindEmptyReg(int endReg);
Symbol GetRegInternal(int width);
int SelectSpillReg(int endReg);

Symbol GetReg();
Symbol GetWordReg();
Symbol GetByteReg();

#endif
