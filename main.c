
#include "Lexer.h"

int main(int argc, char* argv[]){

    if(argc != 2){
        set_color(COLOR_RED, stderr);
        fprintf(stderr, "USAGE: %s <file.txt>\n", argv[0]);
        set_color(0, stderr);
        return 1;
    }

    const char* input_path = argv[1];
    char source[MAX_INPUT_FILE_SIZE] = {'\0'};

    FILE* input = fopen(input_path, "r");
    if(!input){
        set_color(COLOR_RED, stderr);
        fprintf(stderr, "ERROR: cannot open file: `%s` for this reason: %s", input_path, strerror(errno));
        set_color(0, stderr);
        return 1;
    }

    size_t bytes = fread((void*) source, sizeof(*source), sizeof(source) / sizeof(*source), input);
    if(bytes == 0){
        set_color(COLOR_RED, stderr);
        fprintf(stderr, "ERROR: cannot read file: `%s` for this reason: %s", input_path, strerror(errno));
        set_color(0, stderr);
        fclose(input);
        return 1;
    }

    Lexer lex = Lexer_create(source);
    Token tokens[MAX_TOKENS] = {0};
    size_t number_of_tokens = Lexer_lexAllSource(&lex, tokens);
    printf("number of tokens read: %zu\n", number_of_tokens);
    Token_dumbAllTokens(tokens, NULL);
    fclose(input);
    return 0;
}