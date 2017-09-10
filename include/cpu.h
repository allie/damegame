#ifndef CPU_H
#define CPU_H

#include "common.h"
#include "mmu.h"

// Map 8-bit registers depending on host endianness
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define REG_B cpu.reg.byte[0]
	#define REG_C cpu.reg.byte[1]
	#define REG_D cpu.reg.byte[2]
	#define REG_E cpu.reg.byte[3]
	#define REG_H cpu.reg.byte[4]
	#define REG_L cpu.reg.byte[5]
	#define REG_A cpu.reg.byte[6]
	#define REG_F cpu.reg.byte[7]
#else
	#define REG_B cpu.reg.byte[1]
	#define REG_C cpu.reg.byte[0]
	#define REG_D cpu.reg.byte[3]
	#define REG_E cpu.reg.byte[2]
	#define REG_H cpu.reg.byte[5]
	#define REG_L cpu.reg.byte[4]
	#define REG_A cpu.reg.byte[7]
	#define REG_F cpu.reg.byte[6]
#endif

// 16-bit registers
#define REG_BC cpu.reg.word[0]
#define REG_DE cpu.reg.word[1]
#define REG_HL cpu.reg.word[2]
#define REG_AF cpu.reg.word[3]
#define REG_PC cpu.pc
#define REG_SP cpu.sp

// Flags
#define FLAG_Z cpu.z
#define FLAG_N cpu.n
#define FLAG_H cpu.h
#define FLAG_C cpu.c

#define HISTORY_LIMIT 0x10000

typedef struct {
	DWORD m; // Machine cycles
	DWORD t; // Clock periods
} pclock;

typedef struct {
	union {
		BYTE byte[8]; // 8-bit registers
		WORD word[4]; // 16-bit registers
	} reg;

	BYTE z; // Zero flag
	BYTE n; // Subtract flag
	BYTE h; // Half carry flag
	BYTE c; // Carry flag

	WORD pc; // Program counter
	WORD sp; // Stack pointer

	pclock sys_clock; // Master clock
	pclock ins_clock; // Last instruction clock

	BYTE op;    // Current opcode
	BYTE cb_op; // Current opcode (0xCB prefix)
	WORD operand; // Current operand

	int halt; // HALT status
	int stop; // STOP status
	int ime;  // Interrupt mode enabled
} CPU;

typedef struct {
	CPU states[HISTORY_LIMIT];
	WORD ptr;
} CPU_history;

// Executive functions
void CPU_reset(void);
int CPU_run(DWORD);
int CPU_step();

#endif
