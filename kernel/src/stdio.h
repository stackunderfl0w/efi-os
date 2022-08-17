#pragma once
#include "file_struct.h"

extern FILE* stdout;
extern FILE* stdin;

int printf(const char* str, ...);

int sprintf(char* str, const char* format, ...);

int fgetc(FILE* f);

char* fgets(char *str, int n, FILE *f);

int fputc(int c, FILE* f);

int fputs(const char *str, FILE* f);

int fflush(FILE* f);
