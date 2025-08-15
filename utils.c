
#include "utils.h"

void set_color(Color color, FILE* stream) {
    if(!stream){
        stream = stdout;
    }
    switch (color) {
        case COLOR_RED:     fprintf(stream, "\033[0;31m"); break;
        case COLOR_GREEN:   fprintf(stream, "\033[0;32m"); break;
        case COLOR_YELLOW:  fprintf(stream, "\033[0;33m"); break;
        case COLOR_BLUE:    fprintf(stream, "\033[0;34m"); break;
        case COLOR_MAGENTA: fprintf(stream, "\033[0;35m"); break;
        case COLOR_CYAN:    fprintf(stream, "\033[0;36m"); break;
        case COLOR_WHITE:   fprintf(stream, "\033[0;37m"); break;
        case COLOR_RESET:
        default:            fprintf(stream, "\033[0m");    break;
    }
}

const char* Err_to_cstr_table[Err_count+1] = {
    [Err_ok] = "Err_ok",
    [Err_not_implemented] = "Err_not_implemented",

    [Err_count] = "Err_unnon",
};
