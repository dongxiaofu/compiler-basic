#include "common.h"

// TODO 写完才知道，C语言中有和这个函数的功能相同的内置函数strstr。
// 真是没见识！
// 找机会处理掉我写的这个函数。
// 这个函数可能有存在的必要。
int GetSubStrIndex(char *subStr, char *str, unsigned int strLength)
{
	int index = -1;
	
// str可能是 hello\000\world\000。不能使用strlen计算它的真实长度。
//	unsigned int length = strlen(str);
	unsigned int subStrLength = strlen(subStr);

	for(int i = 0; i < strLength; i++){
		// if(strncmp(subStr, str, subStrLength) == 0)){
		// 上面是最初版本。它不能查询num13\000num1。当subStr是num1时，结果是错误的。
		// 不遇到这种数据，我难以想到这种条件语句。
		/***
		 * (gdb) p (subStrLength+str) == '\000'
		*$7 = 0
		*(gdb) p (subStrLength+str)[0] == '\000'
		*$8 = 1
		*(gdb) p *(subStrLength+str) == '\000'
		*$9 = 1
		* * 让我开了眼界。
		* */
		if((strncmp(subStr, str, subStrLength) == 0) && (*(str + subStrLength) == '\000')){
			return i;
		}
		
		str++;
	} 

	return - 1;
}

//Heap CurrentHeap;
//struct heap ProgramHeap;
//HEAP(ProgramHeap);
void StrToUpper(char *str, char *upperStr)
{
	int len = strlen(str);
	for(int i = 0; i < len; i++){
		if(islower(str[i])){
			upperStr[i] = toupper(str[i]);
		}else{
			upperStr[i] = str[i];
		}
	}
}

void StrToLower(char *str, char *lowerStr)
{
	int len = strlen(str);
	for(int i = 0; i < len; i++){
		if(isupper(str[i])){
			lowerStr[i] = tolower(str[i]);
		}else{
			lowerStr[i] = str[i];
		}
	}
}

// 模仿PHP函数命名。
void UcFirst(char *str)
{
	int len = strlen(str);
	if(len == 0)	return;
	str[0] = toupper(str[0]);
}

void *MALLOC(int size)
{
	void *p = (void *)HeapAllocate(CurrentHeap, size);
	memset(p, 0, size);
	return p;
}

int HeapAllocate(Heap heap, int size)
{
	struct mblock *blk = heap->last;

	while(size > blk->end - blk->avail){
		int m = 4096 + sizeof(struct mblock) + size; 
		blk->next = (struct mblock *)malloc(m);
		blk = blk->next;
		if(blk == NULL){
			printf("分配内存失败\n");
			exit(-4);
		}
		blk->begin = blk->avail = (char *)(blk + 1);
//		blk->begin = blk->avail = (char *)(blk);
		blk->next = NULL;
		heap->last = blk;	
		// blk->end = (char *)(blk + m);
		blk->end = (char *)blk + m - 1;
	}

	blk->avail += size;

	// return blk->avail - size;
	return (int)(blk->avail - size);
}
