#ifndef LEXER_H_
#define LEXER_H_

#include "utils.h"

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
void Token_dumbAllTokens(Token tokens[MAX_TOKENS], FILE* stream);

typedef struct Lexer_t
{
    const char* m_source;
    const char* m_start;
    const char* m_end;
    
} Lexer;

Lexer Lexer_create(const char* source);
size_t Lexer_lexAllSource(Lexer* lex, Token tokens[MAX_TOKENS]);
void Lexer_trim(Lexer* lex);
Token Lexer_nextToken(Lexer* lex);
Token Lexer_makeNumber(Lexer* lex);
Token Lexer_makeName(Lexer* lex);
Token Lexer_makeComment(Lexer* lex);
Token Lexer_makeOperation(Lexer* lex);
Token Lexer_makeString(Lexer* lex);
Token Lexer_makeChar(Lexer* lex);


#endif // LEXER_H_
