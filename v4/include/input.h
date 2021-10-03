#ifndef INPUT_H
#define INPUT_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct input{
	char *filename;
	FILE *file;
	int line;
	int size;
	char *cursor;
	char *base;
};

// static struct input Input;
extern struct input Input;

#define CURSOR Input.cursor
#define LINE Input.line


void ReadSourceFile(char *file);

#endif
