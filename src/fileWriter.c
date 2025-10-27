#include <stdio.h>
#include <stdbool.h>
#include "../includes/fileWriter.h"

bool writeFile(const char *filename, char lines[][1024], int numLines) {
    FILE *file = fopen(filename, "w");
    if (!file) return false;

    for (int i = 0; i < numLines; i++) {
        fprintf(file, "%s\n", lines[i]);  // écrit toutes les lignes, même vides
    }

    fclose(file);
    return true;
}
