
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
    Pos pos;
    char* name;
    Code code;
    size_t patch;
    Cell char_value;
} Word;

Word* dict[MAX_DICT_SIZE] = {0};
size_t dict_size = 0;

Word* Word_create(const char* name, Code code, size_t patch, Pos pos, Cell char_value){
    Word* res = malloc(sizeof(*res));
    if(!res){
        return NULL;
    }
    res->name = strdup(name);
    res->code = code;
    res->patch = patch;
    res->pos = pos;
    res->char_value = char_value;
    return res;
}

void Word_dump(Word* word){
    printf("(`%s`, %zu, %zu), 0x%x, `%s`",
            word->pos.file, 
            word->pos.row, 
            word->pos.col,
            (unsigned int)word->patch, 
            word->name);
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

void Dict_insert(Word* dict[MAX_DICT_SIZE], Word* target){
    if(dict_size >= MAX_DICT_SIZE){
        fprintf(stderr, RED "ERROR: to many words in the dictionary\n" RESET);
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

void Stack_push(Cell stack[MAX_STACK_SIZE], Cell value){
    if (sp >= MAX_STACK_SIZE) { 
        fprintf(stderr, RED "RUNTIME ERROR: stack overflow\n" RESET); 
        Dict_clear(dict);
        exit(1); 
    }
    stack[sp++] = value;
}

Cell Stack_pop(Cell stack[MAX_STACK_SIZE]){
    if (sp == 0) { 
        fprintf(stderr, RED "RUNTIME ERROR: stack underflow\n" RESET); 
        Dict_clear(dict);
        exit(1); 
    }
    return stack[--sp];
}

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
    Dict_insert(dict, Word_create("+", Code_add, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("-", Code_sub, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("*", Code_mul, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("/", Code_div, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("%", Code_mod, 0, (Pos){0}, 0));

    // Stack manipulation
    Dict_insert(dict, Word_create("dup", Code_dup, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("drop", Code_drop, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("swap", Code_swap, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("over", Code_over, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("rot", Code_rot, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("-rot", Code_minus_rot, 0, (Pos){0}, 0));

    // Output / I/O
    Dict_insert(dict, Word_create(".", Code_dot, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("emit", Code_emit, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("cr", Code_cr, 0, (Pos){0}, 0));

    // Control Flow
    Dict_insert(dict, Word_create("if", Code_if, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("else", Code_else, 0, (Pos){0}, 0));
    Dict_insert(dict, Word_create("then", Code_then, 0, (Pos){0}, 0));

    // System
    Dict_insert(dict, Word_create("exit", Code_exit, 0, (Pos){0}, 0));

    for(size_t i = 0; i < dict_size; i++){
        if(!dict[i]){
            Dict_clear(dict);
            return false;
        }
    }

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
    Cell char_value = 0;

    while (source[i]) {

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
            if (source[i] == '\n'){
                col = 1;
                row++;
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

        // build token
        char* token = (char*)malloc(sizeof(*token) + 1);
        if(!token){
            fprintf(stderr, RED "ERROR: no memory\n" RESET);
            return false;
        }
        size_t j = 0;
        while (source[i] && !isspace((unsigned char)source[i]) && j < MAX_TOKEN_SIZE - 1) {
            col++;
            token[j++] = source[i++];
        }
        token[j] = '\0';  // make sure token is null-terminated

        // lex chars literals
        if(token[0] == '\'' && token[strlen(token) - 1] == '\'' && (strlen(token) == 3 || strlen(token) == 4)){
            if(strlen(token) == 3) char_value = (Cell)token[1];
            else{
                char_value = escape_to_char(token + 1);
            }
        }

        // build position
        Pos pos = {.file = file_name, .col = col, .row = row};

        raw_words[word_number++] = Word_create(token, NULL, 0, pos, char_value);
        if(!raw_words[word_number - 1]){
            return false;
        }
    }

    return true;
}

bool parser(Word* raw_words[MAX_PROGRAM_SIZE], Word* program[MAX_PROGRAM_SIZE], Word* dict[MAX_DICT_SIZE]){
    Word* word = NULL;
    size_t i = 0;

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
        if (word) {
            if(word->code == Code_if){
                if_stack[if_stack_size++] = program_size;
                if(if_stack_size >= MAX_IF_DEPTH){
                    fprintf(stderr, RED "COMTIME ERROR: maximum number of nested `if` blocks exceeded\n" RESET);
                    return false;
                }
            }
            else if(word->code == Code_then){
                if(if_stack_size == 0){
                    fprintf(stderr, RED "COMTIME ERROR: `then` missing `if` matches\n" RESET);
                    return false;
                }
                patch = if_stack[--if_stack_size]; // fixing `then` patch
                program[patch]->patch = program_size; // fixing `if` patch
            }
            
            program[program_size++] = Word_create(token, word->code, patch, raw_words[i]->pos, raw_words[i]->char_value);
            if(!program[program_size - 1]){
                fprintf(stderr, RED "ERROR: no memory\n" RESET);
                return false;
            }
    
        } else {
            program[program_size++] = Word_create(token, NULL, patch, raw_words[i]->pos, raw_words[i]->char_value); // code==NULL indicate a number literal
            if(!program[program_size - 1]){
                fprintf(stderr, RED "ERROR: no memory\n" RESET);
                return false;
            }
        }
        i++;
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
            long val = strtol(word->name, &end, 0); // base==0 for auto-detect the base
            if (errno == ERANGE) {
                fprintf(stderr, RED "RUNTIME ERROR: number out of range: `%s`\n" RESET, word->name);
                return false;
            }
            if (*end == '\0') {
                Stack_push(stack, (Cell)val);
            }
            else if(word->char_value != 0){
                Stack_push(stack, word->char_value);
            } else { // not a number and not a known word sow run time error
                fprintf(stderr, RED "RUNTIME ERROR: unknown word: `%s`\n" RESET, word->name);
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

    if(argc == 2){
        const char* file_path = argv[1];
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

    if(!lexer(source, raw_words, argv[1])){
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

    // Program_dump(program);  // for debug

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
