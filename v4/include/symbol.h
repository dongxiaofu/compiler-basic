#ifndef SYMBOL_H
#define SYMBOL_H

// #include "grammer.h"
//#include "ast.h"
#include "lex.h"
// #include "grammer.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"
// #include "grammer.h"

// #define SYM_HASH_MASK	127
// #define SEARCH_OUTER_TABLE 1
// 
// #define SYMBOL_COMMON	\
// 	int kind;	\
// 	char *name;	\
// 	int level;	\
// 	union value val;	\
// 	struct symbol *link;	\
// 	struct symbol *next;
// 
// typedef struct symbol{
// 	SYMBOL_COMMON
// } *Symbol;
// 
// typedef struct variableSymbol{
// 	SYMBOL_COMMON
// 	InitData initData;	
// } *VariableSymbol;
// 
// typedef struct functionSymbol{
// 	SYMBOL_COMMON
// 	Symbol params;
// 	Symbol results;
// 	Symbol locals;
// } *FunctionSymbol;
// 
// typedef struct bucketLinker{
// 	Symbol sym;
// 	struct bucketLinker *link;
// } *BucketLinker;
// 
// typedef struct table{
// 	Symbol *buckets;
// //	Symbol buckets[128];
// 	int level;
// 	struct table *outer;
// } *Table;
// enum SYMBOL_KIND {INTERFACE_SYM, OTHER_SYM};

unsigned int SymbolHash(char *name);

void InitSymbolTable();
Symbol AddSymbol(Table tbl, Symbol sym, unsigned int hashKey);
Symbol LookupMethodID(char *name, char *receiverTypeAlias);
Symbol LookupID(char *name);
Symbol LookupFunction(AstFunctionDeclarator fdec);
Symbol LookupSymbol(Table tbl, char *name, char *receiverTypeAlias);
char * GetSymbolKind(int kind);
Symbol DoLookupSymbol(Table tbl, char *name, unsigned int hashKey, int searchOuter);
VariableSymbol AddVariable(char *name, Type ty);
FunctionSymbol AddFunction(char *funcName, Signature sig);

Symbol IntConstant(int i);
Symbol CreateTemp(Type ty);
#endif
