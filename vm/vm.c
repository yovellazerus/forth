
#include "vm.h"

char* Err_to_cstr_table[Err_count] = {
    [Err_ok] = "Err_ok",
    [Err_invalidOpcode] = "Err_invalidOpcode",
    [Err_invalidMod] = "Err_invalidMod",
    [Err_invalidStep] = "Err_invalidStep",
    [Err_divByZero] = "Err_divByZero",
    [Err_segfult] = "Err_segfult",
};


bool VM_is_runing(VM* vm){
    return !(vm->flags & 0x00000001);
}

void VM_dumpMemory(VM* vm)
{
    printf("\nmemory:\n");
    for(word addr = 0; addr < MEMORY_SIZE; addr++){
        if(addr % 8 == 0){
            printf("\n0x%.8x:", addr);
        }
        printf(" 0x%.2x", vm->memory[addr]);
    }
    printf("\n");
}

void VM_dumpProgram(VM* vm, word entry_point, byte* program, size_t program_size)
{
    printf("\nprogram:\n");
    for(word addr = entry_point; addr < entry_point + program_size; addr++){
        if(addr % 8 == 0){
            printf("\n0x%.8x:", addr);
        }
        printf(" 0x%.2x", vm->memory[addr]);
    }
    printf("\n");
}

void VM_dumpCPU(VM* vm)
{
    printf("\nCPU:\n");
    printf("A = 0x%.8x, B = 0x%.8x, C = 0x%.8x, D = 0x%.8x\n",
            vm->a, vm->b, vm->c, vm->d);
    printf("BP = 0x%.8x, SP = 0x%.8x\n",
            vm->bp, vm->sp);
    printf("PC = 0x%.8x, FLAGS = 0x%.8x\n",
            vm->pc, vm->flags);
    printf("FLAGS:\n");
    printf("\th = %d\n", VM_getFlag(vm, 'h'));
    printf("\tz = %d\n", VM_getFlag(vm, 'z'));
    printf("\ts = %d\n", VM_getFlag(vm, 's'));
    printf("\ti = %d\n", VM_getFlag(vm, 'i'));
}

void VM_exeption(VM* vm, Err error){
    if(error == Err_ok){
        return;
    }
    // for now!
    const char* cstr_error = Err_to_cstr_table[error];
    if(!cstr_error){
        cstr_error = "UNKNOWN ERROR";
    }
    
    fprintf(stderr, "\n" RED "ERROR: the virtual machine stopped due to `%s`(error code: %d)" RESET "\n",
                                                                        cstr_error, error);
    
    exit(1);
}

word VM_asembel_word_le(VM* vm){
    word res =
        (word)vm->memory[vm->pc]       |
        ((word)vm->memory[vm->pc + 1] << 8)  |
        ((word)vm->memory[vm->pc + 2] << 16) |
        ((word)vm->memory[vm->pc + 3] << 24);
    vm->pc += 4;
    return res;
}

Err VM_excuteInstruction(VM* vm, Instruction inst){
    if(!inst){
        return Err_invalidOpcode;
    }
    return inst(vm);
}

void VM_setFlag(VM* vm, char flag, bool new_status){
    switch (flag){
        
        case 'h':
            if(new_status == 1){
                vm->flags |= FLAG_HALF;
            }
            else{
                vm->flags &= ~FLAG_HALF;
            }
            break;
        case 'z':
            if(new_status == 1){
                vm->flags |= FLAG_ZERO;
            }
            else{
                vm->flags &= ~FLAG_ZERO;
            }
            break;
        case 's':
            if(new_status == 1){
                vm->flags |= FLAG_SIGN;
            }
            else{
                vm->flags &= ~FLAG_SIGN;
            }
            break;
        case 'i':
            if(new_status == 1){
                vm->flags |= FLAG_INTE;
            }
            else{
                vm->flags &= ~FLAG_INTE;
            }
            break;
            
        default:
            fprintf(stderr, "\n" RED "ERROR: UNKNOWN glag: `%c`\n" RESET, flag);
            exit(1);
    }
}

bool VM_getFlag(VM* vm, char flag){
    switch (flag){
        
        case 'h':
            return vm->flags & 0x00000001;
        case 'z':
            return vm->flags & 0x00000002;
        case 's':
            return vm->flags & 0x00000004;
        case 'i':
            return vm->flags & 0x00000008;
            
        default:
            fprintf(stderr, "\n" RED "ERROR: UNKNOWN glag: `%c`\n" RESET, flag);
            exit(1);
    }
}