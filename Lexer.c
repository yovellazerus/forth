
#include "Lexer.h"

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

#define ERROR(err) do {                                                                             \
    set_color(COLOR_RED, stderr);                                                                   \
    fprintf(stderr, "ERROR: `%s` in file: `%s` in function: `%s` on line: %d.\n",                   \
            Err_to_cstr_table[err] ? Err_to_cstr_table[err] : Err_to_cstr_table[Err_count],         \
            __FILE__, __func__, __LINE__);                                                          \
    set_color(COLOR_RESET, stderr);                                                                 \
    exit(1);                                                                                        \
} while(0)                                                                                          \

const char* TokenType_to_cstr_table[TokenType_count+1] = {
    [TokenType_eof]     = "TokenType_eof",
    [TokenType_error]   = "TokenType_error",
    [TokenType_number]  = "TokenType_number",
    [TokenType_name]    = "TokenType_name",
    [TokenType_comment] = "TokenType_comment",
    [TokenType_string]  = "TokenType_string",
    [TokenType_char]    = "TokenType_char",
    [TokenType_operation]    = "TokenType_operation",

    [TokenType_count] = "TokenType_unknown",
};

Token Token_create(TokenType type, int32_t value, const char *idf_start, size_t idf_size)
{
    return (Token) {.m_type = type, .m_value = value, .m_idf_start = idf_start, .m_idf_size = idf_size};
}

void Token_dump(Token *tok, FILE *stream)
{
    if(!stream){
        stream = stdout;
    }
    if(!tok){
        fprintf(stream, "{null}");
        return;
    }
    fprintf(stream, "{idf: `%.*s`, value: %d, type: `%s`}", 
        (int) tok->m_idf_size,
        tok->m_idf_start, 
        tok->m_value, 
        TokenType_to_cstr_table[tok->m_type] ? TokenType_to_cstr_table[tok->m_type] : TokenType_to_cstr_table[TokenType_count]);
}

Lexer Lexer_create(const char *source)
{
    return (Lexer) {.m_source = source, .m_start = source, .m_end = source};
}

void Lexer_trim(Lexer *lex)
{
    while(*lex->m_end != '\0' && isspace(*lex->m_end)){
        lex->m_end++;
        lex->m_start++;
    }
}

Token Lexer_nextToken(Lexer *lex)
{
    lex->m_start = lex->m_end;
    Lexer_trim(lex);
    if(isdigit(*lex->m_end) 
                || (*lex->m_end == '-' 
                && *(lex->m_end+1) != '\0' && isdigit(*(lex->m_end+1)))){
        return Lexer_makeNumber(lex);
    }
    else if(*lex->m_end == '_' || isalpha(*lex->m_end)){
        return Lexer_makeName(lex);
    }
    else if(*lex->m_end == '\\'){
        return Lexer_makeComment(lex);
    }
    else if(*lex->m_end == '\''){
        return Lexer_makeChar(lex);
    }
    else if(*lex->m_end == '"'){
        return Lexer_makeString(lex);
    }
    else if (*lex->m_end != '\0'){
        return Lexer_makeOperation(lex);
    }

    return Token_create(TokenType_eof, 0, NULL, 0);
}

Token Lexer_makeNumber(Lexer *lex)
{
    ERROR(Err_not_implemented);
    return (Token) {0};
}

Token Lexer_makeName(Lexer *lex)
{
    while (*lex->m_end != '\0' && (isalnum(*lex->m_end) || *lex->m_end == '_')){
        lex->m_end++;
    }
    return Token_create(TokenType_name, 0, lex->m_start, lex->m_end - lex->m_start);
}

Token Lexer_makeComment(Lexer *lex)
{
    ERROR(Err_not_implemented);
    return (Token) {0};
}

Token Lexer_makeOperation(Lexer *lex)
{
    ERROR(Err_not_implemented);
    return (Token) {0};
}

Token Lexer_makeString(Lexer *lex)
{
    ERROR(Err_not_implemented);
    return (Token) {0};
}

Token Lexer_makeChar(Lexer *lex)
{
    ERROR(Err_not_implemented);
    return (Token) {0};
}
