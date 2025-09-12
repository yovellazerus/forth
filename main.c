
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "ansi_colors.h"

#define MAX_FILE_SIZE (1ULL << 16)
#define MAX_TOKEN_SIZE 32

#define MAX_PROGRAM_SIZE (MAX_FILE_SIZE / MAX_TOKEN_SIZE)
#define MAX_DICT_SIZE MAX_PROGRAM_SIZE
#define MAX_STACK_SIZE 1024
#define MAX_IF_DEPTH 1024

#define TRUE 0xffffffff
#define FALSE 0

typedef void (*Code)(void);

typedef int32_t Cell;

typedef struct Pos_t
{
    size_t col;
    size_t row;
    const char *file;
} Pos;

typedef struct Word_t
{
    // better memory usage:
    // const char *name;
    // size_t size;

    char name[MAX_TOKEN_SIZE];
    Code code;
    Pos pos;
    size_t patch;

} Word;

Word dict[MAX_DICT_SIZE] = {0};
size_t dict_size = 0;

void Word_dump(Word *word)
{
    if (word == NULL)
        return;
    printf("word: `%s` pos: (`%s`, %zu, %zu)",
           word->name,
           word->pos.file,
           word->pos.row,
           word->pos.col);
}

Word *Dict_find(Word dict[MAX_DICT_SIZE], const char *name)
{
    for (size_t i = 0; i < dict_size; i++)
    {
        if (strcmp(dict[i].name, name) == 0)
        {
            return &dict[i];
        }
    }
    return NULL;
}

void Dict_dump(Word dict[MAX_DICT_SIZE]){
    printf("dict size: %zu, dict:\n", dict_size);
    printf("{");
    for (size_t i = 0; i < dict_size; i++)
    {
        if(i > 0) printf(", `%s`", dict[i].name);
        else{
            printf("`%s`", dict[i].name);
        }
    }
    printf("}\n");
}

#define ERROR(prifix, msg, word)                                                       \
    do                                                                                 \
    {                                                                                  \
        fprintf(stderr, RED prifix " ERROR: %s, from function: `%s` ", msg, __func__); \
        Word_dump(word);                                                               \
        fprintf(stderr, "\n" RESET);                                                   \
    } while (0)

void Dict_insert(Word dict[MAX_DICT_SIZE], const char* name, Code code)
{
    Word target = {.code = code};
    memcpy(target.name, name, strlen(name) + 1);
    if (dict_size >= MAX_DICT_SIZE)
    {
        ERROR("SYSTEM", "to many words in the dictionary", &target);
        exit(1);
    }
    if (!Dict_find(dict, target.name))
    {
        memcpy(dict[dict_size].name, target.name, strlen(target.name) + 1);
        dict[dict_size].code = target.code;
    }
    else
    {
        return;
    }
    dict_size++;
}

Cell stack[MAX_STACK_SIZE] = {0};
size_t sp = 0;

Word program[MAX_PROGRAM_SIZE] = {0};
size_t program_size = 0;
size_t ip = 0;

void Program_dump(Word program[MAX_PROGRAM_SIZE])
{
    for (size_t i = 0; i < program_size; i++)
    {
        printf("0x%x: ", (unsigned int)i);
        Word_dump(&program[i]);
        printf("\n");
    }
}

void Stack_push(Cell stack[MAX_STACK_SIZE], Cell value)
{
    if (sp >= MAX_STACK_SIZE)
    {
        ERROR("RUNTIME", "stack overflow", &program[ip]);
        exit(1);
    }
    stack[sp++] = value;
}

Cell Stack_pop(Cell stack[MAX_STACK_SIZE])
{
    if (sp == 0)
    {
        ERROR("RUNTIME", "stack underflow", &program[ip]);
        exit(1);
    }
    return stack[--sp];
}

// Arithmetic:

void Code_add()
{
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, a + b);
}

void Code_sub()
{
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b - a);
}

void Code_mul()
{
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b * a);
}

void Code_div()
{
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b / a);
}

void Code_mod()
{
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b % a);
}

// Logic

void Code_true(){
    Stack_push(stack, (Cell)TRUE);
}
void Code_false(){
    Stack_push(stack, 0);
}
void Code_cmp(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    if(a == b){
        Stack_push(stack, TRUE);
    }
    else{
        Stack_push(stack, FALSE);
    }
}
void Code_not_cmp(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    if(a != b){
        Stack_push(stack, TRUE);
    }
    else{
        Stack_push(stack, FALSE);
    }
}
void Code_lt(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    if(a < b){
        Stack_push(stack, TRUE);
    }
    else{
        Stack_push(stack, FALSE);
    }
}
void Code_gt(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    if(a > b){
        Stack_push(stack, TRUE);
    }
    else{
        Stack_push(stack, FALSE);
    }
}

void Code_gte(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    if(a >= b){
        Stack_push(stack, TRUE);
    }
    else{
        Stack_push(stack, FALSE);
    }
}
void Code_lte(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    if(a <= b){
        Stack_push(stack, TRUE);
    }
    else{
        Stack_push(stack, FALSE);
    }
}

// Stack:

void Code_dup()
{
    Cell a = Stack_pop(stack);
    Stack_push(stack, a);
    Stack_push(stack, a);
}

void Code_drop()
{
    Stack_pop(stack);
}

void Code_swap()
{
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b);
    Stack_push(stack, a);
}

void Code_over()
{
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b);
    Stack_push(stack, a);
    Stack_push(stack, b);
}

void Code_rot()
{
    Cell x3 = Stack_pop(stack);
    Cell x2 = Stack_pop(stack);
    Cell x1 = Stack_pop(stack);
    Stack_push(stack, x2);
    Stack_push(stack, x3);
    Stack_push(stack, x1);
}

void Code_minus_rot()
{
    Cell x3 = Stack_pop(stack);
    Cell x2 = Stack_pop(stack);
    Cell x1 = Stack_pop(stack);
    Stack_push(stack, x3);
    Stack_push(stack, x1);
    Stack_push(stack, x2);
}

void Code_emit()
{
    putchar(Stack_pop(stack));
}

void Code_cr()
{
    putchar('\n');
}

// I/O:

void Code_dot()
{
    Cell a = Stack_pop(stack);
    printf("%d\n", a);
}

void Code_key()
{
    Stack_push(stack, (Cell)getchar());
    fflush(stdin);
}

// Control Flow:

void Code_if()
{
    Cell flag = Stack_pop(stack);
    if (flag == 0)
    {
        size_t new_ip = program[ip].patch;
        ip = new_ip;
    }
}

void Code_else()
{
    size_t new_ip = program[ip].patch;
    ip = new_ip;
}

void Code_then()
{
    // no code here `then` is gust a place holder :)
}

/*
    begin -> 0
    while -> repeat
    repeat -> begin
*/

void Code_begin(){

}

void Code_while(){

}

void Code_repeat(){

}

// System:

void Code_exit()
{
    Cell a = Stack_pop(stack);
    exit(a);
}

void Dict_init_default(Word dict[MAX_DICT_SIZE])
{
    // Arithmetic
    Dict_insert(dict, "+", Code_add);
    Dict_insert(dict, "-", Code_sub);
    Dict_insert(dict, "*", Code_mul);
    Dict_insert(dict, "/", Code_div);
    Dict_insert(dict, "%", Code_mod);

    // Logic
    Dict_insert(dict, "true",   Code_true   );
    Dict_insert(dict, "false",  Code_false  );
    Dict_insert(dict, "==",     Code_cmp    );
    Dict_insert(dict, "!=",     Code_not_cmp);
    Dict_insert(dict, "<",      Code_lt     );
    Dict_insert(dict, ">",      Code_gt     );
    Dict_insert(dict, ">=",     Code_gte    );
    Dict_insert(dict, "<=",     Code_lte    );

    // Stack manipulation
    Dict_insert(dict, "dup",  Code_dup      );
    Dict_insert(dict, "drop", Code_drop     );
    Dict_insert(dict, "swap", Code_swap     );
    Dict_insert(dict, "over", Code_over     );
    Dict_insert(dict, "rot",  Code_rot      );
    Dict_insert(dict, "-rot", Code_minus_rot);

    // I/O
    Dict_insert(dict, ".",      Code_dot );
    Dict_insert(dict, "emit",   Code_emit);
    Dict_insert(dict, "cr",     Code_cr  );
    Dict_insert(dict, "key",    Code_key );

    // Control Flow
    Dict_insert(dict, "if",      Code_if    );
    Dict_insert(dict, "else",    Code_else  );
    Dict_insert(dict, "then",    Code_then  );
    Dict_insert(dict, "begin",   Code_begin );
    Dict_insert(dict, "while",   Code_while );
    Dict_insert(dict, "repeat",  Code_repeat);

    // System
    Dict_insert(dict, "exit", Code_exit);

}

size_t if_stack[MAX_IF_DEPTH] = {0};
size_t if_stack_size = 0;

Cell escape_to_char(const char *s)
{
    if (s[0] == '\\')
    {
        switch (s[1])
        {
        case 'n':
            return '\n'; // newline
        case 's':
            return ' '; // space,  TODO: this is my invention not ideal :)
        case 't':
            return '\t'; // tab
        case 'r':
            return '\r'; // carriage return
        case '0':
            return '\0'; // null
        case '\\':
            return '\\'; // backslash
        case '\'':
            return '\''; // single quote
        case '\"':
            return '\"'; // double quote
        default:
            return s[1]; // unknown escape → just return the char
        }
    }
    return s[0]; // not an escape → return literal character
}

int lexer(const char *source, Word raw_words[MAX_PROGRAM_SIZE], const char *file_name)
{

    size_t word_number = 0;
    size_t i = 0;
    size_t col = 1;
    size_t row = 1;
    char token[MAX_TOKEN_SIZE];

    while (source[i])
    {

        if (source[i] == '\n')
        {
            row++;
            col = 1;
        }

        // skip whitespace
        while (isspace((unsigned char)source[i]))
        {
            i++;
            col++;
            if (source[i] == '\n')
            {
                row++;
                col = 1;
            }
        }

        // skip comments: \ ... end of line
        if (source[i] == '\\')
        {
            while (source[i] && source[i] != '\n')
            {
                col++;
                i++;
            }
            continue;
        }
        // skip comments: (...)
        if (source[i] == '(')
        {
            while (source[i] && source[i] != ')')
            {
                if(source[i] == '\n'){
                    col = 1;
                    row++;
                }
                col++;
                i++;
            }
            if(source[i] == '\0'){
                ERROR("COMPILATION", "unterminated comment", &raw_words[word_number]);
                return -1;
            }
            if (source[i] == ')')
            {
                col++;
                i++;
            }
            continue;
        }

        if (source[i] == '\0')
        {
            break;
        }

        // build position
        Pos pos = {.file = file_name, .col = col - 1, .row = row};

        size_t token_len = 0;
        while (source[i] && !isspace((unsigned char)source[i]) && token_len < MAX_TOKEN_SIZE - 1)
        {
            col++;
            token[token_len++] = source[i++];
        }
        token[token_len] = '\0'; // make sure token is null-terminated
        
        memcpy(raw_words[word_number].name, token, token_len + 1);
        raw_words[word_number].code = NULL;
        raw_words[word_number].pos = pos;
        raw_words[word_number].patch = 0;
        if(token_len >= MAX_TOKEN_SIZE - 1){
            ERROR("COMPILATION", "word is to long", &program[word_number]);
            return -1;
        }
        
        word_number++;
        if(word_number >= MAX_PROGRAM_SIZE){
            ERROR("COMPILATION", "program is to long", &raw_words[word_number - 1]);
            return -1;
        }
        memset(token, 0, token_len + 1);
    }

    return word_number;
}

bool parser(Word program[MAX_PROGRAM_SIZE], Word dict[MAX_DICT_SIZE], size_t number_of_words)
{
    Word *word = NULL;
    size_t i = 0;
    Word *bad_if = NULL;
    char token[MAX_TOKEN_SIZE];

    while (i < number_of_words)
    {
        memcpy(token, program[i].name, strlen(program[i].name) + 1);
        /*  lookup the token in the dict.
            If not in the dict then it is a number literal.
            If is in the dict, check if it a Control Flow word, if not addend the word to the program.
            IF it is a Control Flow word, then patch the jump address.
            TODO: make it in to a separate sub routine.
        */
        word = Dict_find(dict, token);
        size_t patch = 0;
        /*
            if -> else (or then)
            else -> then
            then -> 0 (or if)
        */
        if (word)
        {
            if (word->code == Code_if)
            {
                bad_if = &program[i];
                if_stack[if_stack_size++] = program_size;
                if (if_stack_size >= MAX_IF_DEPTH)
                {
                    ERROR("COMPILATION", "maximum number of nested `if` blocks exceeded", &program[i]);
                    return false;
                }
            }
            else if (word->code == Code_else)
            {
                if (if_stack_size == 0)
                {
                    ERROR("COMPILATION", "`else` missing `if` matches", &program[i]);
                    return false;
                }
                patch = if_stack[--if_stack_size];    // geting `if` addr
                program[patch].patch = program_size; // fixing `if` patch
                if (if_stack_size >= MAX_IF_DEPTH)
                {
                    ERROR("COMPILATION", "maximum number of nested `if` blocks exceeded", &program[i]);
                    return false;
                }
                if_stack[if_stack_size++] = program_size; // leave `else`addr on stack for `then` to patch
            }
            else if (word->code == Code_then)
            {
                bad_if = NULL;
                if (if_stack_size == 0)
                {
                    ERROR("COMPILATION", "`then` missing `if` matches", &program[i]);
                    return false;
                }
                patch = if_stack[--if_stack_size];    // geting `else` addr
                program[patch].patch = program_size; // fixing `else` patch to `then` addr
            }

            program[program_size].code = word->code;
            program[program_size].patch = patch;
            program[program_size].pos = program[i].pos;
            memcpy(program[program_size].name, token, strlen(token) + 1);

            program_size++;
        }
        else
        {
            program[program_size].code = NULL; // code==NULL indicate a number literal
            program[program_size].patch = patch;
            program[program_size].pos = program[i].pos;
            memcpy(program[program_size].name, token, strlen(token) + 1);

            program_size++;
        }
        i++;
    }
    if (bad_if)
    {
        ERROR("COMPILATION", "`if` missing `then` matches", bad_if);
        return false;
    }
    return true;
}

bool interpreter(Word program[MAX_PROGRAM_SIZE])
{
    ip = 0;
    while (ip < program_size)
    {
        Word *word = &program[ip];
        if (word->code)
        {
            word->code();
        }
        else
        {
            // try parse number
            char *end;
            errno = 0;
            long value = strtol(word->name, &end, 0); // base==0 for auto-detect the base
            if (errno == ERANGE)
            {
                ERROR("RUNTIME", "number out of range", word);
                return false;
            }
            // pushing a number to the stack
            if (*end == '\0')
            {
                Stack_push(stack, (Cell)value);
            }
            // pushing char literal
            else if (word->name[0] == '\'' && word->name[strlen(word->name) - 1] == '\'' && (strlen(word->name) == 3 || strlen(word->name) == 4))
            {
                if (strlen(word->name) == 3)
                    Stack_push(stack, (Cell)word->name[1]);
                else
                {
                    Stack_push(stack, escape_to_char(word->name + 1));
                }
            }
            else
            { // not a number and not a char literal and not a known word so run time error
                ERROR("RUNTIME", "unkown word", word);
                return false;
            }
        }
        ip++;
    }
    return true;
}

const char* argv_shift(int* argc, char** argv){
    if(*argc == 0) return NULL;
    const char* first = argv[0];
    for(int i = 0; i < *argc - 1; i++){
        argv[i] = argv[i + 1];
    }
    argv[*argc - 1] = NULL; // keep NULL at the end
    (*argc)--;
    return first;
}

int main(int argc, char *argv[])
{
    char source[MAX_FILE_SIZE] = {0};
    bool dprog = false;
    bool ddict = false;

    const char *c_prog_name = argv_shift(&argc, argv);

    const char* file_path = NULL;
    while(argc){
        // flags:
        file_path = argv_shift(&argc, argv);
        if(strcmp(file_path, "-dprog") == 0){
            dprog = true;
        }
        else if(strcmp(file_path, "-ddict") == 0){
            ddict = true;
        }
        // file name:
        else{
            FILE *input = fopen(file_path, "r");
            if (!input)
            {
                fprintf(stderr, RED "ERROR: failure to open the file `%s` due to: `%s`\n" RESET, file_path, strerror(errno));
                return 1;
            }
            size_t file_size = fread(source, sizeof(char), MAX_FILE_SIZE, input);
            source[file_size] = '\0';
            fclose(input);
        }
        
    }
    if(!file_path){
        fprintf(stderr, RED "USAGE: %s [-dprog] [-ddict] <input_file>.forth\n" RESET, c_prog_name);
        fprintf(stderr, RED "ERROR: missing input file\n" RESET);
        return 1;
    }

    Dict_init_default(dict);
    int word_number = lexer(source, program, file_path); // int for error detection
    if(word_number < 0) return 1;
    if (!parser(program, dict, (size_t)word_number)) return 1;
    // for debug
    if(dprog) Program_dump(program); 
    if(ddict) Dict_dump(dict);
    if (!interpreter(program)) return 1;

    return 0;
}
