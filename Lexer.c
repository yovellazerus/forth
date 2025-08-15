
#include "Lexer.h"

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

void Token_dumbAllTokens(Token tokens[MAX_TOKENS], FILE* stream)
{
    if(!stream) stream = stdout;
    if(!tokens) fprintf(stream, "[null]");
    size_t i = 0;
    Token curr = tokens[i];
    while(curr.m_type != TokenType_eof){
        Token_dump(&curr, stream);
        fputc('\n', stream);
        i++;
        curr = tokens[i];
    }
    Token_dump(&curr, stream);
}

Lexer Lexer_create(const char *source)
{
    return (Lexer) {.m_source = source, .m_start = source, .m_end = source};
}

size_t Lexer_lexAllSource(Lexer *lex, Token tokens[MAX_TOKENS])
{
    size_t i = 0;
    Token curr = Token_create(TokenType_error, 0, NULL, 0);
    while(curr.m_type != TokenType_eof){
        curr = Lexer_nextToken(lex);
        tokens[i] = curr;
        i++;
    }
    return i;
}

void Lexer_trim(Lexer *lex)
{
    while(*lex->m_end != '\0' && (isspace(*lex->m_end) || *lex->m_end == '\n')){
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
    int32_t value = (int32_t) strtol(lex->m_start, &lex->m_end, 0);
    return Token_create(TokenType_number, value, lex->m_start, lex->m_end - lex->m_start);
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
    while(*lex->m_end != '\0' && *lex->m_end != '\n'){
        lex->m_end++;
    }
    return Token_create(TokenType_comment, 0, lex->m_start, lex->m_end - lex->m_start);
}

Token Lexer_makeOperation(Lexer *lex)
{
    while (*lex->m_end != '\0' && *lex->m_end != '\n' && !isspace(*lex->m_end))
    {
        lex->m_end++;
    }
    return Token_create(TokenType_operation, 0, lex->m_start, lex->m_end - lex->m_start);
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
