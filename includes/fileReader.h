#ifndef FILEREADER_H
#define FILEREADER_H

#include <stdbool.h>

#define MAX_LINES 100
#define MAX_LEN 1024

bool readFile(const char *filename, char lines[MAX_LINES][MAX_LEN], int *lineCount);

#endif
