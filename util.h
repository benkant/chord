#ifndef _UTIL_H
#define _UTIL_H

#include <string.h>
#include "chord_types.h"

void readRestOfLine();
int getString(char* string, unsigned length, char* prompt);
int getInteger(int* integer, unsigned length, char* prompt, int min, int max);

#endif
