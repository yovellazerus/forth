
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h> 
#include <string.h> 
#include <errno.h> 

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
        fprintf(stderr, "ERROR: to many words in the dictionary\n");
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
        fprintf(stderr, "ERROR: stack overflow\n"); 
        exit(1); 
    }
    stack[sp++] = value;
}

Cell pop(Cell stack[MAX_STACK_SIZE]){
    if (sp == 0) { 
        fprintf(stderr, "ERROR: stack underflow\n"); 
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
                fprintf(stderr, "ERROR: unknown word: `%s`\n", token);
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
            fprintf(stderr, "ERROR: failure to open the file `%s` dow to: %s\n", file_path, strerror(errno));
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
