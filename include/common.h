#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BIG_ENDIAN = 0;
#define LITTLE_ENDIAN = 1;

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int16_t SWORD;
typedef struct {
	BYTE r;
	BYTE g;
	BYTE b;
} RGB;

#endif
