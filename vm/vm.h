#ifndef VM_H_
#define VM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MEMORY_SIZE (1ULL << 16) // 2^16 bytes 64K byte for now

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define FLAG_HALF   (1 << 0)   // lowest bit
#define FLAG_ZERO   (1 << 1)
#define FLAG_SIGN   (1 << 2)
#define FLAG_INTE   (1 << 3)

#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

#define BRIGHT_BLACK   "\033[90m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_WHITE   "\033[97m"

#define RESET   "\033[0m"

typedef uint8_t byte;
typedef uint32_t word;

typedef enum {
    Err_ok = 0,
    Err_invalidOpcode,
    Err_invalidMod,
    Err_invalidStep,
    Err_divByZero,
    Err_segfult,
    
    Err_count,
} Err;

extern char* Err_to_cstr_table[Err_count];

typedef struct VM_t {
    
    byte memory[MEMORY_SIZE];   // little endian
    
    word itr;  // hold the base of the interrupt handler functions, set by the OS at init
    // word seg;
    
    word pc;
    word flags; 
    word sp;
    word bp;        /* flags: (u for undefine)
                    
                    most segbit: u u u u u u u u u u u u u u u u u u u u u u u u u u u u i s z h 
                    
                    */ // for now!!!
    word a;
    word b;
    word c;
    word d;
    
} VM;

typedef enum {
    Intnum_noInt = 0,
    
    Intnum_count,
} Intnum;

typedef enum {
    Mod_nomod = 0,
    
    // for aritmatic and logic ops and mov onlly for r->r and r<->i 
    Mod_ai,
    Mod_bi,
    Mod_ci,
    Mod_di,
    Mod_ia,
    Mod_ib,
    Mod_ic,
    Mod_id,
    Mod_ab,
    Mod_ac,
    Mod_ad,
    Mod_ba,
    Mod_bc,
    Mod_bd,
    Mod_ca,
    Mod_cb,
    Mod_cd,
    Mod_da,
    Mod_db,
    Mod_dc,
    
    // memory addresing mods for load and stor
    Mod_iai,
    Mod_iaa,
    Mod_iab,
    Mod_iac,
    Mod_iad,
    Mod_ibi,
    Mod_iba,
    Mod_ibb,
    Mod_ibc,
    Mod_ibd,
    Mod_ici,
    Mod_ica,
    Mod_icb,
    Mod_icc,
    Mod_icd,
    Mod_idi,
    Mod_ida,
    Mod_idb,
    Mod_idc,
    Mod_idd,
    
    Mod_abi,
    Mod_aba,
    Mod_abb,
    Mod_abc,
    Mod_abd,
    Mod_aci,
    Mod_aca,
    Mod_acb,
    Mod_acc,
    Mod_acd,
    Mod_adi,
    Mod_ada,
    Mod_adb,
    Mod_adc,
    Mod_add,
    
    Mod_bai,
    Mod_baa,
    Mod_bab,
    Mod_bac,
    Mod_bad,
    Mod_bci,
    Mod_bca,
    Mod_bcb,
    Mod_bcc,
    Mod_bcd,
    Mod_bdi,
    Mod_bda,
    Mod_bdb,
    Mod_bdc,
    Mod_bdd,
    
    Mod_cai,
    Mod_caa,
    Mod_cab,
    Mod_cac,
    Mod_cad,
    Mod_cbi,
    Mod_cba,
    Mod_cbb,
    Mod_cbc,
    Mod_cbd,
    Mod_cdi,
    Mod_cda,
    Mod_cdb,
    Mod_cdc,
    Mod_cdd,
    
    Mod_dai,
    Mod_daa,
    Mod_dab,
    Mod_dac,
    Mod_dad,
    Mod_dbi,
    Mod_dba,
    Mod_dbb,
    Mod_dbc,
    Mod_dbd,
    Mod_dci,
    Mod_dca,
    Mod_dcb,
    Mod_dcc,
    Mod_dcd,
    
    // for the stack instructions (???)
    Mod_sp,
    Mod_bp,
    Mod_flags, 
    Mod_pc,

    Mod_ind, // jmp/call indirect
    Mod_abs, // jmp/call absolute
    Mod_pcr, // jmp/call pc reletive
    
    
    Mod_count,
} Mod;

typedef enum {
    // system
    Opcode_nop = 0,
    Opcode_hlt,
    Opcode_int, // triger software interupt
    Opcode_rti, // return from software interupt
    
    // flags
    Opcode_sif,  // diable interupts
    
    // memory
    Opcode_mov, /* mov and all aritmatic and logical intructions are not involving memroy!
                    onlly load and stor are in contect with the memroy, and the stack opertuons...                                                                        */
    Opcode_load,
    Opcode_stor,
    
    // stack
    Opcode_push,
    Opcode_pop,
    
    // aritmatics
    Opcode_add,
    Opcode_sub,
    Opcode_mul,
    Opcode_div,
    Opcode_mod,
    
    // logic
    Opcode_or,
    Opcode_and,
    Opcode_xor,
    Opcode_not,
    
    // shifts
    Opcode_shl,
    Opcode_shr,
    Opcode_sal,
    Opcode_sar,
    Opcode_ror,
    Opcode_rol,
    
    // control flow
    Opcode_cmp,
    Opcode_test,
    
    Opcode_jmp,
    
    Opcode_jz,
    Opcode_jnz,
    Opcode_js,
    Opcode_jns,
    // more needed...
    
    Opcode_call,
    Opcode_ret,
    
    Opcode_count,
} Opcode;

typedef Err (*Instruction)(VM*);

extern Instruction Opcode_to_Instruction_table[Opcode_count];

// VM metods:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void VM_dumpMemory(VM* vm);
void VM_dumpProgram(VM* vm, word entry_point, byte* program, size_t program_size);
void VM_dumpCPU(VM* vm);

bool VM_is_runing(VM* vm);
void VM_setFlag(VM* vm, char flag, bool new_status);
bool VM_getFlag(VM* vm, char flag);

word VM_asembel_word_le(VM* vm);

Err VM_excuteInstruction(VM* vm, Instruction inst);

Intnum VM_lisenForInterupt(VM* vm);
void VM_interupt(VM* vm, Intnum intnum);

void VM_exeption(VM* vm, Err error);

#define VM_APPDATE_FLAGS(vm, reg) do {  \
    if(vm->reg == 0){                   \
        VM_setFlag(vm, 'z', 1);         \
    }                                   \
    else{                               \
        VM_setFlag(vm, 'z', 0);         \
    }                                   \
    if(vm->reg & 0xf000000){            \
        VM_setFlag(vm, 's', 1);         \
    }                                   \
    else{                               \
        VM_setFlag(vm, 's', 0);         \
    }                                   \
} while(0)











#endif // VM_H_
