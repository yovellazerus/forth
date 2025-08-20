
#include "instruction.h"

Instruction Opcode_to_Instruction_table[Opcode_count] = {
    [Opcode_nop]  = inst_nop,
    [Opcode_hlt]  = inst_hlt,
    [Opcode_mov] = inst_mov,
    [Opcode_add]  = inst_add,
    // ...
};

Err inst_nop(VM* vm) { return Err_ok; }
Err inst_hlt(VM* vm) { VM_setFlag(vm, 'h', 1); return Err_ok; }
Err inst_mov(VM* vm) { 
    byte mod = vm->memory[vm->pc++];
    word imm = 0; 
    switch (mod) {
        case Mod_ai: imm = VM_asembel_word_le(vm); vm->a = imm; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_bi: imm = VM_asembel_word_le(vm); vm->b = imm; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_ci: imm = VM_asembel_word_le(vm); vm->c = imm; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_di: imm = VM_asembel_word_le(vm); vm->d = imm; VM_APPDATE_FLAGS(vm, d); break;
        
        case Mod_ab: vm->a = vm->b; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ac: vm->a = vm->c; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ad: vm->a = vm->d; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ba: vm->b = vm->a; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_bc: vm->b = vm->c; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_bd: vm->b = vm->d; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_ca: vm->c = vm->a; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_cb: vm->c = vm->b; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_cd: vm->c = vm->d; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_da: vm->d = vm->a; VM_APPDATE_FLAGS(vm, d); break;
        case Mod_db: vm->d = vm->b; VM_APPDATE_FLAGS(vm, d); break;
        case Mod_dc: vm->d = vm->c; VM_APPDATE_FLAGS(vm, d); break;
        
        default:
            return Err_invalidMod;
            break;
    }
    return Err_ok;
}
Err inst_add(VM* vm) {
    byte mod = vm->memory[vm->pc++];
    word imm = 0;
    switch (mod) {
        case Mod_ai: imm = VM_asembel_word_le(vm); vm->a += imm; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_bi: imm = VM_asembel_word_le(vm); vm->b += imm; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_ci: imm = VM_asembel_word_le(vm); vm->c += imm; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_di: imm = VM_asembel_word_le(vm); vm->d += imm; VM_APPDATE_FLAGS(vm, d); break;
        
        case Mod_ab: vm->a += vm->b; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ac: vm->a += vm->c; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ad: vm->a += vm->d; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ba: vm->b += vm->a; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_bc: vm->b += vm->c; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_bd: vm->b += vm->d; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_ca: vm->c += vm->a; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_cb: vm->c += vm->b; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_cd: vm->c += vm->d; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_da: vm->d += vm->a; VM_APPDATE_FLAGS(vm, d); break;
        case Mod_db: vm->d += vm->b; VM_APPDATE_FLAGS(vm, d); break;
        case Mod_dc: vm->d += vm->c; VM_APPDATE_FLAGS(vm, d); break;
        default:
            return Err_invalidMod;
            break;
    }
    return Err_ok;
}
Err inst_sub(VM* vm) {
    byte mod = vm->memory[vm->pc++];
    word imm = 0;
    switch (mod) {
        case Mod_ai: imm = VM_asembel_word_le(vm); vm->a -= imm; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_bi: imm = VM_asembel_word_le(vm); vm->b -= imm; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_ci: imm = VM_asembel_word_le(vm); vm->c -= imm; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_di: imm = VM_asembel_word_le(vm); vm->d -= imm; VM_APPDATE_FLAGS(vm, d); break;
        
        case Mod_ab: vm->a -= vm->b; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ac: vm->a -= vm->c; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ad: vm->a -= vm->d; VM_APPDATE_FLAGS(vm, a); break;
        case Mod_ba: vm->b -= vm->a; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_bc: vm->b -= vm->c; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_bd: vm->b -= vm->d; VM_APPDATE_FLAGS(vm, b); break;
        case Mod_ca: vm->c -= vm->a; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_cb: vm->c -= vm->b; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_cd: vm->c -= vm->d; VM_APPDATE_FLAGS(vm, c); break;
        case Mod_da: vm->d -= vm->a; VM_APPDATE_FLAGS(vm, d); break;
        case Mod_db: vm->d -= vm->b; VM_APPDATE_FLAGS(vm, d); break;
        case Mod_dc: vm->d -= vm->c; VM_APPDATE_FLAGS(vm, d); break;
        default:
            return Err_invalidMod;
            break;
    }
    return Err_ok;
}

