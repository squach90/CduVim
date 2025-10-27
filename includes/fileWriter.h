#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <stdbool.h>

bool writeFile(const char *filename, char lines[][1024], int numLines);

#endif
