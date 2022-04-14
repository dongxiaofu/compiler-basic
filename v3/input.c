#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "input.h"
#include "lex.h"

struct input Input;

void ReadSourceFile(char *file){

	// 获取文件大小 size
	Input.file = fopen("1.go", "r");
	if(Input.file == NULL){
		fclose(Input.file);
		perror("open file error\n");
		return;
	}
	fseek(Input.file, 0, SEEK_END);
	Input.size = ftell(Input.file);
	// 开辟一段size大小的内存空间存储源码,这段内存空间的初始地址是base
	// todo 为何要加1？
	Input.base = (char *)malloc(Input.size + 1);
	if(Input.base == -1){
		
		fclose(Input.file);
		perror("the file is too big\n");
		return;
	}
	// 把源码读取到内存中，内存中的源码数据的初始地址是base
	fseek(Input.file, 0, SEEK_SET);
	fread(Input.base, 1, Input.size + 1, Input.file);
	fclose(Input.file);

	Input.line = 1;
	Input.base[Input.size + 1] = EOF;
	Input.cursor = Input.base;
}
