#pragma once
#include "file_struct.h"
void sprintf(char* str, const char* format, ...);

void printf(const char* str, ...);

extern FILE* stdout;

void new_printf(const char* str, ...);

int fgetc(FILE* f);

int fputc(int c, FILE* f);

int fputs(const char *str, FILE* f);

