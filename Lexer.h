#ifndef LEXER_H_
#define LEXER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

typedef enum {
    Err_ok = 0,
    Err_not_implemented,

    Err_count,
} Err;

typedef enum {
    TokenType_eof = 0,
    TokenType_error,
    TokenType_number,
    TokenType_name,
    TokenType_comment,
    TokenType_string,
    TokenType_char,
    TokenType_operation,

    TokenType_count,
} TokenType;

typedef struct Token_t 
{
    TokenType m_type;
    int32_t m_value;
    const char* m_idf_start;
    size_t m_idf_size;
    
} Token;

Token Token_create(TokenType type, int32_t value, const char* idf_start, size_t idf_size);
void Token_dump(Token* tok, FILE* stream);

typedef struct Lexer_t
{
    const char* m_source;
    const char* m_start;
    const char* m_end;
    
} Lexer;

Lexer Lexer_create(const char* source);
void Lexer_trim(Lexer* lex);
Token Lexer_nextToken(Lexer* lex);
Token Lexer_makeNumber(Lexer* lex);
Token Lexer_makeName(Lexer* lex);
Token Lexer_makeComment(Lexer* lex);
Token Lexer_makeOperation(Lexer* lex);
Token Lexer_makeString(Lexer* lex);
Token Lexer_makeChar(Lexer* lex);









#endif // LEXER_H_
