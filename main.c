
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h> 
#include <string.h> 
#include <errno.h> 

#include "ansi_colors.h"

#define MAX_STACK_SIZE 1024
#define MAX_DICT_SIZE 1024
#define MAX_PROGRAM_SIZE 1024
#define MAX_TOKEN_SIZE 32
#define MAX_IF_DEPTH 1024
#define MAX_FILE_SIZE (1ULL << 16)

typedef void (*Code)(void);

typedef int32_t Cell;

typedef struct Pos_t {
    size_t col;
    size_t row;
    const char* file;
} Pos;

typedef struct Word_t {
    
    char* name;
    Code code;

    Pos pos;
    size_t patch;

    size_t patch_if;
    size_t patch_else;
    size_t patch_then;

} Word;

Word* dict[MAX_DICT_SIZE] = {0};
size_t dict_size = 0;

Word* Word_create(const char* name, Code code, Pos pos){
    Word* res = malloc(sizeof(*res));
    if(!res){
        return NULL;
    }
    res->name = strdup(name);
    res->code = code;
    res->pos = pos;
    return res;
}

void Word_dump(Word* word){
    if(word == NULL) return;
    printf("word: `%s` pos: (`%s`, %zu, %zu)",
            word->name,
            word->pos.file, 
            word->pos.row, 
            word->pos.col
            );
}

Word* Dict_find(Word* dict[MAX_DICT_SIZE], const char* name){
    for(size_t i = 0; i < dict_size; i++){
        if(strcmp(dict[i]->name, name) == 0){
            return dict[i];
        }
    }
    return NULL;
}

void Dict_clear(Word* dict[MAX_DICT_SIZE]){
    for(size_t i = 0; i < dict_size; i++){
        free(dict[i]->name);
    }
    dict_size = 0;
}

#define ERROR(prifix, msg, word) do {                                                       \
    fprintf(stderr, RED prifix " ERROR: %s, from function: `%s` ", msg, __func__);          \
    Word_dump(word);                                                                        \
    fprintf(stderr, "\n" RESET);                                                            \
} while(0)

void Dict_insert(Word* dict[MAX_DICT_SIZE], Word* target){
    if(dict_size >= MAX_DICT_SIZE){
        ERROR("SYSTEM", "to many words in the dictionary", target);
        Dict_clear(dict);
        exit(1);
    }
    if(!Dict_find(dict, target->name)){
        dict[dict_size++] = target;
    }
    else{
        return;
    }
}

Cell stack[MAX_STACK_SIZE] = {0};
size_t sp = 0;

Word* program[MAX_PROGRAM_SIZE] = {0};
size_t program_size = 0;
size_t ip = 0;

void Program_dump(Word* program[MAX_PROGRAM_SIZE]){
    for(size_t i = 0; i < program_size; i++){
       printf("0x%x: ", (unsigned int) i); Word_dump(program[i]); printf("\n");
    }
}

void Program_clear(Word* program[MAX_PROGRAM_SIZE]){
    for(size_t i = 0; i < program_size; i++){
        free(program[i]->name);
    }
    program_size = 0;
}

void Stack_push(Cell stack[MAX_STACK_SIZE], Cell value){
    if (sp >= MAX_STACK_SIZE) { 
        ERROR("RUNTIME", "stack overflow", program[ip]); 
        Dict_clear(dict);
        Program_clear(program);
        exit(1); 
    }
    stack[sp++] = value;
}

Cell Stack_pop(Cell stack[MAX_STACK_SIZE]){
    if (sp == 0) { 
        ERROR("RUNTIME", "stack underflow", program[ip]); 
        Dict_clear(dict);
        Program_clear(program);
        exit(1); 
    }
    return stack[--sp];
}

// Arithmetic:

void Code_add(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, a + b);
}

void Code_sub(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b - a);
}

void Code_mul(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b * a);
}

void Code_div(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b / a);
}

void Code_mod(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b % a);
}

// Stack:

void Code_dup(){
    Cell a = Stack_pop(stack);
    Stack_push(stack, a);
    Stack_push(stack, a);
}

void Code_drop(){
    Stack_pop(stack);
}

void Code_swap(){
    Cell a = Stack_pop(stack);
    Cell b = Stack_pop(stack);
    Stack_push(stack, b);
    Stack_push(stack, a);
}

void Code_over() {
    Cell a = Stack_pop(stack); 
    Cell b = Stack_pop(stack); 
    Stack_push(stack, b);    
    Stack_push(stack, a);    
    Stack_push(stack, b);      
}

void Code_rot(){
    Cell x3 = Stack_pop(stack);
    Cell x2 = Stack_pop(stack);
    Cell x1 = Stack_pop(stack);
    Stack_push(stack, x2);
    Stack_push(stack, x3);
    Stack_push(stack, x1);
}

void Code_minus_rot(){
    Cell x3 = Stack_pop(stack); 
    Cell x2 = Stack_pop(stack); 
    Cell x1 = Stack_pop(stack); 
    Stack_push(stack, x3); 
    Stack_push(stack, x1); 
    Stack_push(stack, x2); 
}

void Code_emit(){ 
    putchar(Stack_pop(stack)); 
}

void Code_cr(){ 
    putchar('\n'); 
}

// I/O:

void Code_dot(){
    Cell a = Stack_pop(stack);
    printf("%d\n", a);
}

// Control Flow:

void Code_if(){
    Cell flag = Stack_pop(stack);
    if(flag == 0){
        size_t new_ip = program[ip]->patch;
        ip = new_ip;
    }
}

void Code_else(){
    size_t new_ip = program[ip]->patch;
    ip = new_ip;
}

void Code_then(){
    // no code here `then` is gust a place holder :)
}

// System:

void Code_exit(){
    Cell a = Stack_pop(stack);
    Dict_clear(dict);
    exit(a);
}

bool Dict_init_default(Word* dict[MAX_DICT_SIZE]){
    // Arithmetic
    Dict_insert(dict, Word_create("+", Code_add, (Pos){0}));
    Dict_insert(dict, Word_create("-", Code_sub, (Pos){0}));
    Dict_insert(dict, Word_create("*", Code_mul, (Pos){0}));
    Dict_insert(dict, Word_create("/", Code_div, (Pos){0}));
    Dict_insert(dict, Word_create("%", Code_mod, (Pos){0}));

    // Stack manipulation
    Dict_insert(dict, Word_create("dup", Code_dup, (Pos){0}));
    Dict_insert(dict, Word_create("drop", Code_drop, (Pos){0}));
    Dict_insert(dict, Word_create("swap", Code_swap, (Pos){0}));
    Dict_insert(dict, Word_create("over", Code_over, (Pos){0}));
    Dict_insert(dict, Word_create("rot", Code_rot, (Pos){0}));
    Dict_insert(dict, Word_create("-rot", Code_minus_rot, (Pos){0}));

    // Output / I/O
    Dict_insert(dict, Word_create(".", Code_dot, (Pos){0}));
    Dict_insert(dict, Word_create("emit", Code_emit, (Pos){0}));
    Dict_insert(dict, Word_create("cr", Code_cr, (Pos){0}));

    // Control Flow
    Dict_insert(dict, Word_create("if", Code_if, (Pos){0}));
    Dict_insert(dict, Word_create("else", Code_else, (Pos){0}));
    Dict_insert(dict, Word_create("then", Code_then, (Pos){0}));

    // System
    Dict_insert(dict, Word_create("exit", Code_exit, (Pos){0}));

    return true;
}

size_t if_stack[MAX_IF_DEPTH] = {0};
size_t if_stack_size = 0;

void Raw_words_clear(Word* raw_words[MAX_PROGRAM_SIZE]){
    size_t i = 0;
    while (raw_words[i])
    {
        free(raw_words[i]->name);
        free(raw_words[i]);
        i++;
    }
}

Cell escape_to_char(const char *s) {
    if (s[0] == '\\') {
        switch (s[1]) {
            case 'n': return '\n';  // newline
            case 's': return ' ';   // space,  TODO: this is my invention not ideal :)
            case 't': return '\t';  // tab
            case 'r': return '\r';  // carriage return
            case '0': return '\0';  // null
            case '\\': return '\\'; // backslash
            case '\'': return '\''; // single quote
            case '\"': return '\"'; // double quote
            default: return s[1];   // unknown escape → just return the char
        }
    }
    return s[0];  // not an escape → return literal character
}

bool lexer(const char* source, Word* raw_words[MAX_PROGRAM_SIZE], const char* file_name){

    size_t word_number = 0;
    size_t i = 0;
    size_t col = 1;
    size_t row = 1;

    while (source[i]) {

        if(source[i] == '\n'){
            row++;
            col = 1;
        }

        // skip whitespace
        while (isspace((unsigned char)source[i])) {
            i++;
            col++;
            if(source[i] == '\n'){
                row++;
                col = 1;
            }
        }

        // skip comments: \ ... end of line
        if (source[i] == '\\') {
            while (source[i] && source[i] != '\n'){
                col++;
                i++;
            } 
            continue;
        }
        // skip comments: (...)
        if (source[i] == '(') {
            while (source[i] && source[i] != ')'){
                col++;
                i++;
            }
            if (source[i] == ')'){
                col++;
                i++;
            }
            continue;
        }

        if (source[i] == '\0') {
            break;
        }

        // build position
        Pos pos = {.file = file_name, .col = col - 1, .row = row};

        // build token
        char* token = (char*)malloc(sizeof(*token) + 1);
        if(!token){
            ERROR("SYSTEM", "no memory", NULL);
            return false;
        }
        size_t j = 0;
        while (source[i] && !isspace((unsigned char)source[i]) && j < MAX_TOKEN_SIZE - 1) {
            col++;
            token[j++] = source[i++];
        }
        token[j] = '\0';  // make sure token is null-terminated

        raw_words[word_number++] = Word_create(token, NULL, pos);
        if(!raw_words[word_number - 1]){
            return false;
        }
    }

    return true;
}

bool parser(Word* raw_words[MAX_PROGRAM_SIZE], Word* program[MAX_PROGRAM_SIZE], Word* dict[MAX_DICT_SIZE]){
    Word* word = NULL;
    size_t i = 0;
    Word* bad_if = NULL;

    while(raw_words[i]){
        const char* token = raw_words[i]->name;
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
        if (word) {
            if(word->code == Code_if){
                bad_if = raw_words[i];
                if_stack[if_stack_size++] = program_size;
                if(if_stack_size >= MAX_IF_DEPTH){
                    ERROR("COMPILATION", "maximum number of nested `if` blocks exceeded", raw_words[i]);
                    return false;
                }
            }
            else if(word->code == Code_else){
                if(if_stack_size == 0){
                    ERROR("COMPILATION", "`else` missing `if` matches", raw_words[i]);
                    return false;
                }
                patch = if_stack[--if_stack_size]; // geting `if` addr
                program[patch]->patch = program_size; // fixing `if` patch
                if(if_stack_size >= MAX_IF_DEPTH){
                    ERROR("COMPILATION", "maximum number of nested `if` blocks exceeded", raw_words[i]);
                    return false;
                }
                if_stack[if_stack_size++] = program_size; // leave `else`addr on stack for `then` to patch
            }
            else if(word->code == Code_then){
                bad_if = NULL;
                if(if_stack_size == 0){
                    ERROR("COMPILATION", "`then` missing `if` matches", raw_words[i]);
                    return false;
                }
                patch = if_stack[--if_stack_size]; // geting `else` addr
                program[patch]->patch = program_size; // fixing `else` patch to `then` addr
            }
            
            program[program_size++] = Word_create(token, word->code, raw_words[i]->pos);
            if(!program[program_size - 1]){
                ERROR("SYSTEM", "no memory", raw_words[i]);
                return false;
            }
            program[program_size - 1]->patch = patch;
    
        } else {
            program[program_size++] = Word_create(token, NULL, raw_words[i]->pos); // code==NULL indicate a number literal
            if(!program[program_size - 1]){
                ERROR("SYSTEM", "no memory", raw_words[i]);
                return false;
            }
            program[program_size - 1]->patch = patch;
        }
        i++;
    }
    if(bad_if){
        ERROR("COMPILATION", "`if` missing `then` matches", bad_if);
        return false;
    }
    return true;
}

bool interpreter(Word* program[MAX_PROGRAM_SIZE]){
    ip = 0;
    while(ip < program_size){
        Word* word = program[ip];
        if(word->code){
            word->code();
        }
        else{
            // try parse number
            char *end;
            errno = 0;
            long value = strtol(word->name, &end, 0); // base==0 for auto-detect the base
            if (errno == ERANGE) {
                ERROR("RUNTIME", "number out of range", word);
                return false;
            }
            // pushing a number to the stack
            if (*end == '\0') {
                Stack_push(stack, (Cell)value);
            }
            // pushing char literal
            else if(word->name[0] == '\'' && word->name[strlen(word->name) - 1] == '\'' && (strlen(word->name) == 3 || strlen(word->name) == 4)){
                if(strlen(word->name) == 3) Stack_push(stack, (Cell)word->name[1]);
                else{
                    Stack_push(stack, escape_to_char(word->name + 1));
                }
            }
            else { // not a number and not a char literal and not a known word so run time error
                ERROR("RUNTIME", "unkown word", word);
                return false;
            }
        }
        ip++;
    }
    return true;
}

int main(int argc, char* argv[]){

    char source[MAX_FILE_SIZE]; 
    Word* raw_words[MAX_PROGRAM_SIZE] = {0};
    const char* file_path = NULL;

    if(argc == 2){
        file_path = argv[1];
        FILE* input = fopen(file_path, "r");
        if(!input){
            fprintf(stderr, RED "ERROR: failure to open the file `%s` due to: `%s`\n" RESET, file_path, strerror(errno));
            return 1;
        }
        size_t file_size = fread(source, sizeof(char), MAX_FILE_SIZE, input);
        source[file_size] = '\0';
        fclose(input);
    }
    else{
        fprintf(stderr, RED "USAGE: forth <input_file>.forth\n" RESET);
        fprintf(stderr, RED "ERROR: missing input file\n" RESET);
        return 1;
    }

    if(!Dict_init_default(dict)){
        Dict_clear(dict);
        Program_clear(program);
        Raw_words_clear(raw_words);
        return 1;
    }

    if(!lexer(source, raw_words, file_path)){
        Dict_clear(dict);
        Program_clear(program);
        Raw_words_clear(raw_words);
        return 1;
    }

    if(!parser(raw_words, program, dict)){
        Dict_clear(dict);
        Program_clear(program);
        Raw_words_clear(raw_words);
        return 1;
    }

    Program_dump(program);  // for debug

    if(!interpreter(program)){
        Dict_clear(dict);
        Program_clear(program);
        Raw_words_clear(raw_words);
        return 1;
    }

    Dict_clear(dict);
    Program_clear(program);
    Raw_words_clear(raw_words);
    return 0;
}
