#pragma once
#include "file_struct.h"

extern FILE* stdout;

void printf(const char* str, ...);

void sprintf(char* str, const char* format, ...);

void old_printf(const char* str, ...);


int fgetc(FILE* f);

int fputc(int c, FILE* f);

int fputs(const char *str, FILE* f);

