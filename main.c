
#include "Lexer.h"

int main(int argc, char* argv[]){

    const char* source = "    my name is yovel lazerus and   i am 27 years   old  ";
    Lexer lexer = Lexer_create(source);
    Token curr = Token_create(TokenType_error, 0, NULL, 0);
    while(curr.m_type != TokenType_eof){
        curr = Lexer_nextToken(&lexer);
        Token_dump(&curr, NULL);
        putchar('\n');
    }

    return 0;
}