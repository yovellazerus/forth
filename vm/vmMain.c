
#include "vm.h"

int main(int argc, char* argv[])
{
	VM vm = {0};
	
	//printf("%d\n", Mod_count);
	
	byte program[] = {
	  Opcode_mov, Mod_ai, 0x12, 0x34, 0x56, 0x78,
	  Opcode_mov, Mod_bi, 0x01, 0x00, 0x00, 0x00,
	  
	  // swap d<->c using b
	  Opcode_mov, Mod_ci, 0x0c, 0x00, 0x00, 0x00,
	  Opcode_mov, Mod_di, 0x0d, 0x00, 0x00, 0x00,
	  Opcode_mov, Mod_bc,
	  Opcode_mov, Mod_cd,
	  Opcode_mov, Mod_db,
	  
	  Opcode_nop,
	  Opcode_nop,
	  Opcode_nop,
	  
	  Opcode_add, Mod_ai, 0x02, 0x00, 0x00, 0xff,
	  Opcode_mov, Mod_ai, 0x00, 0x00, 0x00, 0x00,
	  Opcode_mov, Mod_bi, 0x0b, 0x00, 0x00, 0x00,
	  Opcode_mov, Mod_ai, 0x00, 0x00, 0x00, 0x00,
	  Opcode_nop,
	  Opcode_nop,
	  Opcode_hlt,
	};
	word program_entry_point = 0;
	size_t program_size = sizeof(program);
	
	memcpy(vm.memory + program_entry_point, program, program_size);
	vm.pc = program_entry_point;
	
	while(VM_is_runing(&vm)){
	    
	    // Intnum intnum =  VM_lisenForInterupt(&vm); // if no interupt: intnum = Intnum_noInt = 0
	    
	    // VM_interupt(&vm, intnum); // if intnum = Intnum_noInt do nothing
	    
	    // fech
	    Opcode opcode = vm.memory[vm.pc++];
	    
	    // decode
	    Instruction inst = Opcode_to_Instruction_table[opcode]; // for invalid opcode returns NULL
	    
	    // excute
	    Err error = VM_excuteInstruction(&vm, inst); // if inst=NULL than returns Err_invalidOpcode
	    
	    VM_exeption(&vm, error); // if error=Err_ok=0 do nothing
	    
	}
	
	VM_dumpProgram(&vm, program_entry_point, program, program_size);
    VM_dumpCPU(&vm);

	return 0;
}
