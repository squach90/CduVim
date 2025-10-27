#include "../includes/fileReader.h"
#include <stdio.h>
#include <string.h>

bool readFile(const char *filename, char lines[MAX_LINES][MAX_LEN], int *lineCount) {
    FILE *file = fopen(filename, "r");
    if (!file) return false;

    *lineCount = 0;
    while (fgets(lines[*lineCount], MAX_LEN, file)) {
        size_t len = strlen(lines[*lineCount]);
        if (len > 0 && lines[*lineCount][len - 1] == '\n') {
            lines[*lineCount][len - 1] = '\0';
        }
        (*lineCount)++;
        if (*lineCount >= MAX_LINES) break;
    }

    fclose(file);
    return true;
}
