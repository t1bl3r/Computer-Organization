/** @file logic.c
 *  @brief Implementation of the logic.h interface
 *  @details This is a skeleton implementation of the interface that you will write
 *  for the assignment. The skeleton provides functions to fetch, decode, and execute 
 *  instructions as well as the ADD immediate and ST instructions.  This allows you
 *  to run the simulator with a simple test program before starting to verify your
 *  setup. You must complete the remainder of the instructions.
 *  <p>
 *  @author <b>your name here</b>
 */

#include "lc3.h"
#include "hardware.h"
#include "logic.h"

static int not_implemented() {
  return (! OK);
}


/* Instruction fetch, decode, and execution functions already provided. 
 *
 * logic_fetch_instruction completes the first 3 clock cycles of each
 * instruction that load the IR from PC address and increments PC.
 *
 * logic_decode_instruction verifies instruction and extracts components 
 * from the IR used by the instructions and performs appropriate sign extensions.
 *
 * logic_execute_instruction calls the appropriate routine to complete
 * the remaining cycles for the specified opcode.  
 */

void logic_fetch_instruction (instruction_t* inst) {
  /* clock cycle 1 */
  hardware_gate_PC();             /* put PC onto BUS   */
  hardware_load_MAR();            /* load MAR from BUS */
  hardware_set_PC(*lc3_BUS+1);    /* increment PC      */
  inst->addr = *lc3_BUS;          /* save PC for inst  */
  /* clock cycle 2 */
  hardware_memory_enable(0);      /* read memory       */
  /* clock cycle 3 */
  hardware_gate_MDR();            /* put MDR on BUS    */
  inst->bits = *lc3_BUS;          /* load IR from BUS  */

}

int logic_decode_instruction (instruction_t* inst) {
  int valid   = OK; /* valid instruction */
  int instVal = inst->bits;

  /*  Extract the components from the instruction (instVal) */

  inst->opcode              = (instVal >> 12) & 0xF;

  
  inst->DR                  = (instVal >> 9) & 0x7;
  inst->SR1                 = (instVal >> 6) & 0x7;
  inst->SR2                 = instVal & 0x7;
  inst->bit5                = (instVal >> 5) & 0x1;
  inst->bit11               = (instVal >> 11) & 0x1;
  inst->trapvect8           = instVal & 0xff;
  inst->imm5                = (instVal & 0x1f) << (32-5) >> (32-5);
  inst->offset6             = (instVal & 0x3f) <<(32-6) >> (32-6);
  inst->PCoffset9           = (instVal & 0x1ff) <<(32-9) >> (32-9);
  inst->PCoffset11          = (instVal & 0x7ff) << (32-11) >> (32-11);
   
  /* check for invalid instructions (i.e. fields which must be all 0's or 1's */

  switch (inst->opcode) {
    
    case OP_BR:
      if (inst->opcode != 0x0)
        valid = !OK;
      break;
    
    case OP_ADD:
    case OP_AND: 
      if (inst->bit5 == 0x0)
       if (((instVal>>3)&0x3) != 0x0)
        valid = !OK;
      break;

    case OP_JSR_JSRR:
      if (inst->bit11 == 0x0)
       if((instVal & 0x3F) != 0x0) 
        valid = !OK;
      break;

    case OP_RTI:
      if ((instVal & 0xfff) != 0x0 )
        valid = !OK;
      break;

    case OP_NOT:
      if ((instVal & 0x3f) != 0x3f )
        valid = !OK;
      break;

    case OP_JMP_RET:
      if ((instVal & 0x3f) != 0x0 )
        valid = !OK;
      break;

    case OP_RESERVED:
      valid = !OK; // RESERVED not used
      break;

    case OP_TRAP:
      if ((instVal >> 8) !=0x00f0)
        valid = !OK;
      break;
 
    default: /* LEA, LD, LDI, LDR, ST, STI STR, ... have no additional checks */
      break;
  }

  return valid;
}


/** @todo implement each instruction */
static int logic_NZP(LC3_WORD value) {
	if (value > 32767)
	 return 4;
        if (value > 0)
	 return 1;
	return 2; 
}


static int execute_NOT (instruction_t* inst) {
	LC3_WORD ALU = hardware_get_REG(inst->SR1);
	ALU = ~ALU;
	lc3_BUS = &ALU;
	hardware_load_REG(inst->DR); 
	hardware_set_CC(logic_NZP(ALU));         
	return 0;
}

static int execute_ADD (instruction_t* inst) {
	if (inst->bit5 == 0x0) {
 	 LC3_WORD S1 = hardware_get_REG(inst->SR1);
	 LC3_WORD S2 = hardware_get_REG(inst->SR2);
	 S1 = S1 + S2;
	 lc3_BUS = &S1;
	 hardware_load_REG(inst->DR);
	 hardware_set_CC(logic_NZP(S1)); }
	if (inst->bit5 == 0x1) {
	 LC3_WORD val = hardware_get_REG(inst->SR1);
	 val = val + inst->imm5;
	 lc3_BUS = &val;
	 hardware_load_REG(inst->DR);
	 hardware_set_CC(logic_NZP(val)); }
	 
	return 0;
}

static int execute_AND (instruction_t* inst) { 
	if (inst->bit5 == 0x0) {
	 LC3_WORD S1 = hardware_get_REG(inst->SR1);
	 LC3_WORD S2 = hardware_get_REG(inst->SR2);
	 S1 = (S1 & S2);
	 lc3_BUS = &S1;
	 hardware_load_REG(inst->DR);
	 hardware_set_CC(logic_NZP(S1)); }
	if (inst->bit5 == 0x1) {
	 LC3_WORD val = hardware_get_REG(inst->SR1);
	 val = val & inst->imm5;
	 lc3_BUS = &val; 
	 hardware_load_REG(inst->DR);
	 hardware_set_CC(logic_NZP(val)); }

	return 0;
}

static int execute_BR (instruction_t* inst) {
	unsigned short NZP = hardware_get_CC();
	LC3_WORD  newPC = hardware_get_PC() + inst->PCoffset9;
	unsigned short check = inst->bits; 
	check = ((check >> 9) & 0x7);
	if (NZP == 4) {
	 if ((check == 4) || (check == 5) || (check == 6) || (check == 7)) {
	  hardware_set_PC(newPC);
	  return 0; } }
	if (NZP == 2) {
	 if ((check == 3) || (check == 2) || (check == 6) || (check == 7)) {
	  hardware_set_PC(newPC);
	  return 0; } }
	if (NZP == 1) {
	 if ((check == 1) || (check == 3) || (check == 5) || (check == 7)) {
	  hardware_set_PC(newPC);
	  return 0; } } 
	if (check == 0) {
	 hardware_set_PC(newPC);
	 return 0; }
	return 0; 
}
	 
static int execute_JMP (instruction_t* inst) {
	LC3_WORD check = inst->bits; 
	check = (check >> 6) & 0x7;
	hardware_set_PC(hardware_get_REG(check));
	return 0;
}

static int execute_LD (instruction_t* inst) {
	LC3_WORD offset = inst->PCoffset9;
	offset = offset + hardware_get_PC(); 
	lc3_BUS = &offset;
	hardware_load_MAR(); 
	
	hardware_memory_enable(0);
	
	hardware_gate_MDR();
	hardware_load_REG(inst->DR); 
	hardware_set_CC(logic_NZP(*lc3_BUS));
	return 0;
}

static int execute_TRAP (instruction_t* inst) {
	LC3_WORD vect = inst->trapvect8;
	lc3_BUS = &vect;
	hardware_load_MAR();
	
	hardware_memory_enable(0);
	LC3_WORD load = hardware_get_PC();
	lc3_BUS = &load;
	hardware_load_REG(7); 

	hardware_gate_MDR();
	hardware_set_PC(*lc3_BUS);
	
	return 0;
}

static int execute_ST(instruction_t* inst) {
	LC3_WORD currPC = hardware_get_PC();
	currPC = currPC + inst->PCoffset9;
	lc3_BUS = &currPC;
	hardware_load_MAR();

	LC3_WORD regVal = hardware_get_REG(inst->DR);
	lc3_BUS = &regVal;
	hardware_load_MDR();

	hardware_memory_enable(1);
	return OK;
}

static int execute_JSR_JSRR(instruction_t* inst) {
	if (inst->bit11 == 1) {
	 LC3_WORD currPC = hardware_get_PC();
	 lc3_BUS = &currPC;
	 hardware_load_REG(7);

	 currPC = currPC + (inst->PCoffset11);
	 hardware_set_PC(currPC); 
	
	 return OK; }
	if (inst->bit11 == 0) {
	 LC3_WORD currPC = hardware_get_PC();
	 lc3_BUS = &currPC;
	 hardware_load_REG(7);

	 hardware_gate_REG(inst->SR1);
	 hardware_set_PC(*lc3_BUS);
	 return OK; } 
return 1;
} 

static int execute_LDR(instruction_t* inst) {
	LC3_WORD sr1 = hardware_get_REG(inst->SR1);
	sr1 = sr1 + inst->offset6;
	lc3_BUS = &sr1;
	hardware_load_MAR();

	hardware_memory_enable(0);

	hardware_gate_MDR();
	hardware_load_REG(inst->DR);
	hardware_set_CC(logic_NZP(hardware_get_REG(inst->DR)));

	return OK;
}

static int execute_STR(instruction_t* inst) {
	LC3_WORD sr1 = hardware_get_REG(inst->SR1);
	LC3_WORD both = sr1 + inst->offset6;
	lc3_BUS = &both;
	hardware_load_MAR();
	
	LC3_WORD SR = hardware_get_REG(inst->DR);
	lc3_BUS = &SR;
	hardware_load_MDR();

	hardware_memory_enable(1);
	
	return OK;
}

static int execute_STI(instruction_t* inst) {
	LC3_WORD val1 = hardware_get_PC() + (inst->PCoffset9);
	lc3_BUS = &val1;
	hardware_load_MAR();

	hardware_memory_enable(0);

	hardware_gate_MDR();
	hardware_load_MAR();

	LC3_WORD sr1 = hardware_get_REG(inst->DR);
	lc3_BUS = &sr1;
	hardware_load_MDR();

	hardware_memory_enable(1);
	
	return OK;
} 

static int execute_RTI(instruction_t* inst) {
	hardware_gate_PSR();
	LC3_WORD SP = *lc3_BUS;
	if ((SP & 0x8000) == 0){
	 LC3_WORD r6 = hardware_get_REG(6);
	 hardware_set_PC(r6);
	 r6 = r6 + 1;
	 lc3_BUS = &r6;
	 hardware_load_PSR();
	 r6 = r6 + 1;
	 lc3_BUS = &r6;
	 hardware_load_REG(6);
	 return 0; }
	return 1;
}

static int execute_LDI(instruction_t* inst) {
	LC3_WORD val1 = hardware_get_PC() + inst->PCoffset9;
	lc3_BUS = &val1;
	hardware_load_MAR();

	hardware_memory_enable(0);

	hardware_gate_MDR();
	hardware_load_MAR();

	hardware_memory_enable(0);

	hardware_gate_MDR();
	val1 = *lc3_BUS;
	hardware_load_REG(inst->DR);
	hardware_set_CC(logic_NZP(val1));
	return OK;
} 

static int execute_LEA(instruction_t* inst) {
	LC3_WORD val1 = hardware_get_PC() + inst->PCoffset9;
	lc3_BUS = &val1;
	hardware_load_REG(inst->DR);

	hardware_set_CC(logic_NZP(val1));

	return OK;
}
	
int logic_execute_instruction (instruction_t* inst) {
  switch (inst->opcode) {
    case OP_BR:       return execute_BR(inst);
    case OP_ADD:      return execute_ADD(inst);
    case OP_LD:       return execute_LD(inst);
    case OP_ST:       return execute_ST(inst);
    case OP_JSR_JSRR: return execute_JSR_JSRR(inst);
    case OP_AND:      return execute_AND(inst);
    case OP_LDR:      return execute_LDR(inst);
    case OP_STR:      return execute_STR(inst);
    case OP_RTI:      return execute_RTI(inst);
    case OP_NOT:      return execute_NOT(inst);
    case OP_LDI:      return execute_LDI(inst);
    case OP_STI:      return execute_STI(inst);
    case OP_JMP_RET:  return execute_JMP(inst);
    case OP_RESERVED: return not_implemented();
    case OP_LEA:      return execute_LEA(inst);
    case OP_TRAP:     return execute_TRAP(inst);
    default:          return not_implemented();
  }

  return (! OK);
}


