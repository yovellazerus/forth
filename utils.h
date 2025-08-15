#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <errno.h>


#define MAX_TOKENS (1024)
#define MAX_INPUT_FILE_SIZE (256*256)
#define MAX_OPERATION_INF (32)

typedef enum {
    COLOR_RESET = 0,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_COUNT,
} Color;

void set_color(Color color, FILE* stream);

#define ERROR(err) do {                                                                             \
    set_color(COLOR_RED, stderr);                                                                   \
    fprintf(stderr, "ERROR: `%s` in file: `%s` in function: `%s()` on line: %d.\n",                 \
            Err_to_cstr_table[err] ? Err_to_cstr_table[err] : Err_to_cstr_table[Err_count],         \
            __FILE__, __func__, __LINE__);                                                          \
    set_color(COLOR_RESET, stderr);                                                                 \
    exit(1);                                                                                        \
} while(0)                                                                                          \

typedef enum {
    Err_ok = 0,
    Err_not_implemented,

    Err_count,
} Err;

extern const char* Err_to_cstr_table[Err_count+1];

#endif // UTILS_H_
