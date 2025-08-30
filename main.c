
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h> 
#include <string.h> 
#include <errno.h> 

// Reset
#define RESET       "\x1b[0m"

// Regular colors
#define BLACK       "\x1b[30m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"
#define WHITE       "\x1b[37m"

// Bright colors
#define BRIGHT_BLACK   "\x1b[90m"
#define BRIGHT_RED     "\x1b[91m"
#define BRIGHT_GREEN   "\x1b[92m"
#define BRIGHT_YELLOW  "\x1b[93m"
#define BRIGHT_BLUE    "\x1b[94m"
#define BRIGHT_MAGENTA "\x1b[95m"
#define BRIGHT_CYAN    "\x1b[96m"
#define BRIGHT_WHITE   "\x1b[97m"

// Background colors
#define BG_BLACK     "\x1b[40m"
#define BG_RED       "\x1b[41m"
#define BG_GREEN     "\x1b[42m"
#define BG_YELLOW    "\x1b[43m"
#define BG_BLUE      "\x1b[44m"
#define BG_MAGENTA   "\x1b[45m"
#define BG_CYAN      "\x1b[46m"
#define BG_WHITE     "\x1b[47m"

// Bright background colors
#define BG_BRIGHT_BLACK   "\x1b[100m"
#define BG_BRIGHT_RED     "\x1b[101m"
#define BG_BRIGHT_GREEN   "\x1b[102m"
#define BG_BRIGHT_YELLOW  "\x1b[103m"
#define BG_BRIGHT_BLUE    "\x1b[104m"
#define BG_BRIGHT_MAGENTA "\x1b[105m"
#define BG_BRIGHT_CYAN    "\x1b[106m"
#define BG_BRIGHT_WHITE   "\x1b[107m"

// Styles
#define BOLD        "\x1b[1m"
#define DIM         "\x1b[2m"
#define UNDERLINE   "\x1b[4m"
#define BLINK       "\x1b[5m"
#define REVERSE     "\x1b[7m"
#define HIDDEN      "\x1b[8m"


#define MAX_STACK_SIZE 1024
#define MAX_LINE_SIZE 1024
#define MAX_WORDS 1024
#define MAX_TOKEN_SIZE 32
#define MAX_FILE_SIZE (1ULL << 16)

typedef void (*Code)(void);

typedef int32_t Cell;

typedef struct Word_t {
    const char* name;
    Code code;
} Word;

Word* dict[MAX_WORDS] = {0};
size_t word_count = 0;

Word* Word_create(const char* name, Code code){
    Word* res = malloc(sizeof(*res));
    if(!res){
        return NULL;
    }
    res->name = strdup(name);
    res->code = code;
    return res;
}

Word* Word_find(Word* dict[MAX_WORDS], const char* name){
    for(size_t i = 0; i < word_count; i++){
        if(strcmp(dict[i]->name, name) == 0){
            return dict[i];
        }
    }
    return NULL;
}
void Word_insert(Word* dict[MAX_WORDS], Word* target){
    if(word_count >= MAX_WORDS){
        fprintf(stderr, RED "ERROR: to many words in the dictionary\n" RESET);
        Word_clear(dict);
        exit(1);
    }
    if(!Word_find(dict, target->name)){
        dict[word_count++] = target;
    }
    else{
        return;
    }
}

void Word_clear(Word* dict[MAX_WORDS]){
    for(size_t i = 0; i < word_count; i++){
        free(dict[i]->name);
    }
    word_count = 0;
}

Cell stack[MAX_STACK_SIZE] = {0};
size_t sp = 0;

void push(Cell stack[MAX_STACK_SIZE], Cell value){
    if (sp >= MAX_STACK_SIZE) { 
        fprintf(stderr, RED "ERROR: stack overflow\n" RESET); 
        Word_clear(dict);
        exit(1); 
    }
    stack[sp++] = value;
}

Cell pop(Cell stack[MAX_STACK_SIZE]){
    if (sp == 0) { 
        fprintf(stderr, RED "ERROR: stack underflow\n" RESET); 
        Word_clear(dict);
        exit(1); 
    }
    return stack[--sp];
}

// Arithmetic:

void Code_add(){
    Cell a = pop(stack);
    Cell b = pop(stack);
    push(stack, a + b);
}

void Code_sub(){
    Cell a = pop(stack);
    Cell b = pop(stack);
    push(stack, b - a);
}

void Code_mul(){
    Cell a = pop(stack);
    Cell b = pop(stack);
    push(stack, b * a);
}

void Code_div(){
    Cell a = pop(stack);
    Cell b = pop(stack);
    push(stack, b / a);
}

void Code_mod(){
    Cell a = pop(stack);
    Cell b = pop(stack);
    push(stack, b % a);
}

// Stack:

void Code_dup(){
    Cell a = pop(stack);
    push(stack, a);
    push(stack, a);
}
void Code_drop(){
    pop(stack);
}
void Code_swap(){
    Cell a = pop(stack);
    Cell b = pop(stack);
    push(stack, b);
    push(stack, a);
}

void Code_over(){
    Cell x3 = pop(stack);
    Cell x2 = pop(stack);
    Cell x1 = pop(stack);
    push(stack, x2);
    push(stack, x3);
    push(stack, x1);

} 

void Code_rot(){
    Cell x3 = pop(stack);
    Cell x2 = pop(stack);
    Cell x1 = pop(stack);
    push(stack, x2);
    push(stack, x3);
    push(stack, x1);
}

// I/O:

void Code_dot(){
    Cell a = pop(stack);
    printf("%d\n", a);
}

// System:

void Code_exit(){
    Cell a = pop(stack);
    Word_clear(dict);
    exit(a);
}

void parser(const char* source, Word* dict[MAX_WORDS]){
    char token[MAX_TOKEN_SIZE] = {'\0'};
    Word* target = NULL;
    size_t i = 0;

    while (source[i]) {

        // skip whitespace
        while (isspace((unsigned char)source[i])) {
            i++;
        }

        // skip comments ( \ ... end of line )
        if (source[i] == '\\') {
            while (source[i] && source[i] != '\n') i++;
            if (source[i] == '\n') i++;
            continue;
        }

        if (source[i] == '\0') {
            break;
        }

        // build token
        size_t j = 0;
        while (source[i] && !isspace((unsigned char)source[i]) && j < MAX_TOKEN_SIZE - 1) {
            token[j++] = source[i++];
        }
        token[j] = '\0';  // make sure token is null-terminated

        // lookup
        target = Word_find(dict, token);
        if (target) {
            target->code();
        } else {
            // try parse number
            char *end;
            long val = strtol(token, &end, 0); // base==0 for auto-detect the base
            if (*end == '\0') {
                push(stack, (Cell)val);
            } else {
                fprintf(stderr, RED "ERROR: unknown word: `%s`\n" RESET, token);
                Word_clear(dict);
                exit(1);
            }
        }
    }
}


int main(int argc, char* argv[]){

    char source[MAX_FILE_SIZE] = {'\0'}; 
    char line[MAX_LINE_SIZE] = {'\0'};
    bool cmdline = true;

    if(argc > 1){
        const char* file_path = argv[1];
        FILE* input = fopen(file_path, "r");
        if(!input){
            fprintf(stderr, RED "ERROR: failure to open the file `%s` dow to: `%s`\n" RESET, file_path, strerror(errno));
            Word_clear(dict);
            return 1;
        }
        fread(source, sizeof(char), MAX_FILE_SIZE, input);
        fclose(input);
        cmdline = false;
    }
    else{
        cmdline = true;
    }

    Word_insert(dict, Word_create("+", Code_add));
    Word_insert(dict, Word_create("-", Code_sub));
    Word_insert(dict, Word_create("*", Code_mul));
    Word_insert(dict, Word_create("/", Code_div));
    Word_insert(dict, Word_create("%", Code_mod));
    Word_insert(dict, Word_create(".", Code_dot));
    Word_insert(dict, Word_create("exit", Code_exit));
    Word_insert(dict, Word_create("dup", Code_dup));
    Word_insert(dict, Word_create("swap", Code_swap));
    Word_insert(dict, Word_create("over", Code_over));
    Word_insert(dict, Word_create("rot", Code_rot));

    if(cmdline){
        printf("\nWelcome to the Forth programming language interpreter,\n");
        printf("for exting the interpreter type `0 exit`\n");
        while(true){
            printf(">>> ");
            fgets(line, MAX_LINE_SIZE, stdin);
            for(int k = 0; k < MAX_LINE_SIZE; k++){
                if(line[k] == '\n') line[k] = '\0';
            }
            parser(line, dict);
        }
    }
    else{
        parser(source, dict);
    }

    Word_clear(dict);
    
    return 0;
}
