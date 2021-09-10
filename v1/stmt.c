#include "lex.h"
#include "stmt.h"

int main(int argc, char *argv[])
{
	fp = fopen("test.c", "r");
	if(fp == NULL){
		perror("open test.c error");
		exit(-1);
	}

	//NEXT_TOKEN;
	compound_stmt();

	return 0;
}

void compound_stmt()
{
	NEXT_TOKEN;
	// 希望是{
	EXPECT(TK_LBRACE);
	// 中间是其他语句，把它们放在一个单链表中。
	// 希望是}
	EXPECT(TK_RBRACE);

}
