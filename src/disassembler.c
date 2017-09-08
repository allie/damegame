#include "disassembler.h"
#include "cpu.h"
#include "mmu.h"
#include <stdio.h>

extern CPU cpu;
extern MMU mmu;

void Disassembler_log() {

}

void Disassembler_print(char* buf, size_t size) {
	switch (cpu.op) {
	case 0x00: snprintf(buf, size, "%s", "nop"); break;
	case 0x01: snprintf(buf, size, "%s0x%04X", "ld bc,", cpu.operand); break;
	case 0x02: snprintf(buf, size, "%s0x%04X", "ld (bc),", cpu.operand); break;
	case 0x03: snprintf(buf, size, "%s", "inc bc"); break;
	case 0x04: snprintf(buf, size, "%s", "inc b"); break;
	case 0x05: snprintf(buf, size, "%s", "dec b"); break;
	case 0x06: snprintf(buf, size, "%s0x%02X", "ld b,", cpu.operand); break;
	case 0x07: snprintf(buf, size, "%s", "rlca"); break;
	case 0x08: snprintf(buf, size, "%s0x%04X%s", "ld (", cpu.operand, "),sp"); break;
	case 0x09: snprintf(buf, size, "%s", "add hl,bc"); break;
	case 0x0A: snprintf(buf, size, "%s", "ld a,(bc)"); break;
	case 0x0B: snprintf(buf, size, "%s", "dec bc"); break;
	case 0x0C: snprintf(buf, size, "%s", "inc c"); break;
	case 0x0D: snprintf(buf, size, "%s", "dec c"); break;
	case 0x0E: snprintf(buf, size, "%s0x%02X", "ld c,", cpu.operand); break;
	case 0x0F: snprintf(buf, size, "%s", "rrca"); break;

	case 0x10: snprintf(buf, size, "%s", "stop"); break;
	case 0x11: snprintf(buf, size, "%s0x%04X", "ld de,", cpu.operand); break;
	case 0x12: snprintf(buf, size, "%s0x%04X", "ld (de),", cpu.operand); break;
	case 0x13: snprintf(buf, size, "%s", "inc de"); break;
	case 0x14: snprintf(buf, size, "%s", "inc d"); break;
	case 0x15: snprintf(buf, size, "%s", "dec d"); break;
	case 0x16: snprintf(buf, size, "%s0x%02X", "ld d,", cpu.operand); break;
	case 0x17: snprintf(buf, size, "%s", "rla"); break;
	case 0x18: snprintf(buf, size, "%s0x%04X", "jr ", cpu.operand); break;
	case 0x19: snprintf(buf, size, "%s", "add hl,de"); break;
	case 0x1A: snprintf(buf, size, "%s", "ld a,(de)"); break;
	case 0x1B: snprintf(buf, size, "%s", "dec de"); break;
	case 0x1C: snprintf(buf, size, "%s", "inc e"); break;
	case 0x1D: snprintf(buf, size, "%s", "dec e"); break;
	case 0x1E: snprintf(buf, size, "%s0x%02X", "ld e,", cpu.operand); break;
	case 0x1F: snprintf(buf, size, "%s", "rra"); break;

	case 0x21: snprintf(buf, size, "%s0x%04X", "ld hl,", cpu.operand); break;
	case 0x22: snprintf(buf, size, "%s", "ldi (hl),a"); break;
	case 0x23: snprintf(buf, size, "%s", "inc hl"); break;
	case 0x24: snprintf(buf, size, "%s", "inc h"); break;
	case 0x25: snprintf(buf, size, "%s", "dec h"); break;
	case 0x26: snprintf(buf, size, "%s0x%02X", "ld h,", cpu.operand); break;
	case 0x27: snprintf(buf, size, "%s", "daa"); break;
	case 0x28: snprintf(buf, size, "%s0x%04X", "jrz ", cpu.operand); break;
	case 0x29: snprintf(buf, size, "%s", "add hl,hl"); break;
	case 0x2A: snprintf(buf, size, "%s", "ldi a,(hl)"); break;
	case 0x2B: snprintf(buf, size, "%s", "dec hl"); break;
	case 0x2C: snprintf(buf, size, "%s", "inc l"); break;
	case 0x2D: snprintf(buf, size, "%s", "dec l"); break;
	case 0x2E: snprintf(buf, size, "%s0x%02X", "ld l,", cpu.operand); break;
	case 0x2F: snprintf(buf, size, "%s", "cpl"); break;

	case 0x31: snprintf(buf, size, "%s0x%04X", "ld sp,", cpu.operand); break;
	case 0x32: snprintf(buf, size, "%s", "ldd (hl),a"); break;
	case 0x33: snprintf(buf, size, "%s", "inc sp"); break;
	case 0x34: snprintf(buf, size, "%s", "inc (hl)"); break;
	case 0x35: snprintf(buf, size, "%s", "dec (hl)"); break;
	case 0x36: snprintf(buf, size, "%s0x%02X", "ld (hl),", cpu.operand); break;
	case 0x37: snprintf(buf, size, "%s", "scf"); break;
	case 0x38: snprintf(buf, size, "%s0x%04X", "jrc ", cpu.operand); break;
	case 0x39: snprintf(buf, size, "%s", "add hl,sp"); break;
	case 0x3A: snprintf(buf, size, "%s", "ldd a,(hl)"); break;
	case 0x3B: snprintf(buf, size, "%s", "dec sp"); break;
	case 0x3C: snprintf(buf, size, "%s", "inc a"); break;
	case 0x3D: snprintf(buf, size, "%s", "dec a"); break;
	case 0x3E: snprintf(buf, size, "%s0x%02X", "ld a,", cpu.operand); break;
	case 0x3F: snprintf(buf, size, "%s", "ccf"); break;

	case 0x40: snprintf(buf, size, "%s", "ld b,b"); break;
	case 0x41: snprintf(buf, size, "%s", "ld b,c"); break;
	case 0x42: snprintf(buf, size, "%s", "ld b,d"); break;
	case 0x43: snprintf(buf, size, "%s", "ld b,e"); break;
	case 0x44: snprintf(buf, size, "%s", "ld b,h"); break;
	case 0x45: snprintf(buf, size, "%s", "ld b,l"); break;
	case 0x46: snprintf(buf, size, "%s", "ld b,(hl)"); break;
	case 0x47: snprintf(buf, size, "%s", "ld b,a"); break;
	case 0x48: snprintf(buf, size, "%s", "ld c,b"); break;
	case 0x49: snprintf(buf, size, "%s", "ld c,c"); break;
	case 0x4A: snprintf(buf, size, "%s", "ld c,d"); break;
	case 0x4B: snprintf(buf, size, "%s", "ld c,e"); break;
	case 0x4C: snprintf(buf, size, "%s", "ld c,h"); break;
	case 0x4D: snprintf(buf, size, "%s", "ld c,l"); break;
	case 0x4E: snprintf(buf, size, "%s", "ld c,(hl)"); break;
	case 0x4F: snprintf(buf, size, "%s", "ld c,a"); break;

	case 0x50: snprintf(buf, size, "%s", "ld d,b"); break;
	case 0x51: snprintf(buf, size, "%s", "ld d,c"); break;
	case 0x52: snprintf(buf, size, "%s", "ld d,d"); break;
	case 0x53: snprintf(buf, size, "%s", "ld d,e"); break;
	case 0x54: snprintf(buf, size, "%s", "ld d,h"); break;
	case 0x55: snprintf(buf, size, "%s", "ld d,l"); break;
	case 0x56: snprintf(buf, size, "%s", "ld d,(hl)"); break;
	case 0x57: snprintf(buf, size, "%s", "ld d,a"); break;
	case 0x58: snprintf(buf, size, "%s", "ld e,b"); break;
	case 0x59: snprintf(buf, size, "%s", "ld e,c"); break;
	case 0x5A: snprintf(buf, size, "%s", "ld e,d"); break;
	case 0x5B: snprintf(buf, size, "%s", "ld e,e"); break;
	case 0x5C: snprintf(buf, size, "%s", "ld e,h"); break;
	case 0x5D: snprintf(buf, size, "%s", "ld e,l"); break;
	case 0x5E: snprintf(buf, size, "%s", "ld e,(hl)"); break;
	case 0x5F: snprintf(buf, size, "%s", "ld e,a"); break;

	case 0x60: snprintf(buf, size, "%s", "ld h,b"); break;
	case 0x61: snprintf(buf, size, "%s", "ld h,c"); break;
	case 0x62: snprintf(buf, size, "%s", "ld h,d"); break;
	case 0x63: snprintf(buf, size, "%s", "ld h,e"); break;
	case 0x64: snprintf(buf, size, "%s", "ld h,h"); break;
	case 0x65: snprintf(buf, size, "%s", "ld h,l"); break;
	case 0x66: snprintf(buf, size, "%s", "ld h,(hl)"); break;
	case 0x67: snprintf(buf, size, "%s", "ld h,a"); break;
	case 0x68: snprintf(buf, size, "%s", "ld l,b"); break;
	case 0x69: snprintf(buf, size, "%s", "ld l,c"); break;
	case 0x6A: snprintf(buf, size, "%s", "ld l,d"); break;
	case 0x6B: snprintf(buf, size, "%s", "ld l,e"); break;
	case 0x6C: snprintf(buf, size, "%s", "ld l,h"); break;
	case 0x6D: snprintf(buf, size, "%s", "ld l,l"); break;
	case 0x6E: snprintf(buf, size, "%s", "ld l,(hl)"); break;
	case 0x6F: snprintf(buf, size, "%s", "ld l,a"); break;

	case 0x70: snprintf(buf, size, "%s", "ld (hl),b"); break;
	case 0x71: snprintf(buf, size, "%s", "ld (hl),c"); break;
	case 0x72: snprintf(buf, size, "%s", "ld (hl),d"); break;
	case 0x73: snprintf(buf, size, "%s", "ld (hl),e"); break;
	case 0x74: snprintf(buf, size, "%s", "ld (hl),h"); break;
	case 0x75: snprintf(buf, size, "%s", "ld (hl),l"); break;
	case 0x76: snprintf(buf, size, "%s", "halt"); break;
	case 0x77: snprintf(buf, size, "%s", "ld (hl),a"); break;
	case 0x78: snprintf(buf, size, "%s", "ld a,b"); break;
	case 0x79: snprintf(buf, size, "%s", "ld a,c"); break;
	case 0x7A: snprintf(buf, size, "%s", "ld a,d"); break;
	case 0x7B: snprintf(buf, size, "%s", "ld a,e"); break;
	case 0x7C: snprintf(buf, size, "%s", "ld a,h"); break;
	case 0x7D: snprintf(buf, size, "%s", "ld a,l"); break;
	case 0x7E: snprintf(buf, size, "%s", "ld a,(hl)"); break;
	case 0x7F: snprintf(buf, size, "%s", "ld a,a"); break;

	case 0x80: snprintf(buf, size, "%s", "add a,b"); break;
	case 0x81: snprintf(buf, size, "%s", "add a,c"); break;
	case 0x82: snprintf(buf, size, "%s", "add a,d"); break;
	case 0x83: snprintf(buf, size, "%s", "add a,e"); break;
	case 0x84: snprintf(buf, size, "%s", "add a,h"); break;
	case 0x85: snprintf(buf, size, "%s", "add a,l"); break;
	case 0x86: snprintf(buf, size, "%s", "add a,(hl)"); break;
	case 0x87: snprintf(buf, size, "%s", "add a,a"); break;
	case 0x88: snprintf(buf, size, "%s", "adc a,b"); break;
	case 0x89: snprintf(buf, size, "%s", "adc a,c"); break;
	case 0x8A: snprintf(buf, size, "%s", "adc a,d"); break;
	case 0x8B: snprintf(buf, size, "%s", "adc a,e"); break;
	case 0x8C: snprintf(buf, size, "%s", "adc a,h"); break;
	case 0x8D: snprintf(buf, size, "%s", "adc a,l"); break;
	case 0x8E: snprintf(buf, size, "%s", "adc a,(hl)"); break;
	case 0x8F: snprintf(buf, size, "%s", "adc a,a"); break;

	case 0x90: snprintf(buf, size, "%s", "sub a,b"); break;
	case 0x91: snprintf(buf, size, "%s", "sub a,c"); break;
	case 0x92: snprintf(buf, size, "%s", "sub a,d"); break;
	case 0x93: snprintf(buf, size, "%s", "sub a,e"); break;
	case 0x94: snprintf(buf, size, "%s", "sub a,h"); break;
	case 0x95: snprintf(buf, size, "%s", "sub a,l"); break;
	case 0x96: snprintf(buf, size, "%s", "sub a,(hl)"); break;
	case 0x97: snprintf(buf, size, "%s", "sub a,a"); break;
	case 0x98: snprintf(buf, size, "%s", "sbc a,b"); break;
	case 0x99: snprintf(buf, size, "%s", "sbc a,c"); break;
	case 0x9A: snprintf(buf, size, "%s", "sbc a,d"); break;
	case 0x9B: snprintf(buf, size, "%s", "sbc a,e"); break;
	case 0x9C: snprintf(buf, size, "%s", "sbc a,h"); break;
	case 0x9D: snprintf(buf, size, "%s", "sbc a,l"); break;
	case 0x9E: snprintf(buf, size, "%s", "sbc a,(hl)"); break;
	case 0x9F: snprintf(buf, size, "%s", "sbc a,a"); break;

	case 0xA0: snprintf(buf, size, "%s", "and b"); break;
	case 0xA1: snprintf(buf, size, "%s", "and c"); break;
	case 0xA2: snprintf(buf, size, "%s", "and d"); break;
	case 0xA3: snprintf(buf, size, "%s", "and e"); break;
	case 0xA4: snprintf(buf, size, "%s", "and h"); break;
	case 0xA5: snprintf(buf, size, "%s", "and l"); break;
	case 0xA6: snprintf(buf, size, "%s", "and (hl)"); break;
	case 0xA7: snprintf(buf, size, "%s", "and a"); break;
	case 0xA8: snprintf(buf, size, "%s", "xor b"); break;
	case 0xA9: snprintf(buf, size, "%s", "xor c"); break;
	case 0xAA: snprintf(buf, size, "%s", "xor d"); break;
	case 0xAB: snprintf(buf, size, "%s", "xor e"); break;
	case 0xAC: snprintf(buf, size, "%s", "xor h"); break;
	case 0xAD: snprintf(buf, size, "%s", "xor l"); break;
	case 0xAE: snprintf(buf, size, "%s", "xor (hl)"); break;
	case 0xAF: snprintf(buf, size, "%s", "xor a"); break;

	case 0xB0: snprintf(buf, size, "%s", "or b"); break;
	case 0xB1: snprintf(buf, size, "%s", "or c"); break;
	case 0xB2: snprintf(buf, size, "%s", "or d"); break;
	case 0xB3: snprintf(buf, size, "%s", "or e"); break;
	case 0xB4: snprintf(buf, size, "%s", "or h"); break;
	case 0xB5: snprintf(buf, size, "%s", "or l"); break;
	case 0xB6: snprintf(buf, size, "%s", "or (hl)"); break;
	case 0xB7: snprintf(buf, size, "%s", "or a"); break;
	case 0xB8: snprintf(buf, size, "%s", "cp b"); break;
	case 0xB9: snprintf(buf, size, "%s", "cp c"); break;
	case 0xBA: snprintf(buf, size, "%s", "cp d"); break;
	case 0xBB: snprintf(buf, size, "%s", "cp e"); break;
	case 0xBC: snprintf(buf, size, "%s", "cp h"); break;
	case 0xBD: snprintf(buf, size, "%s", "cp l"); break;
	case 0xBE: snprintf(buf, size, "%s", "cp (hl)"); break;
	case 0xBF: snprintf(buf, size, "%s", "cp a"); break;

	case 0xC0: snprintf(buf, size, "%s", "ret nz"); break;
	case 0xC1: snprintf(buf, size, "%s", "pop bc"); break;
	case 0xC2: snprintf(buf, size, "%s0x%04X", "jp nz,", cpu.operand); break;
	case 0xC3: snprintf(buf, size, "%s0x%04X", "jp ", cpu.operand); break;
	case 0xC4: snprintf(buf, size, "%s0x%04X", "call nz,", cpu.operand); break;
	case 0xC5: snprintf(buf, size, "%s", "push bc"); break;
	case 0xC6: snprintf(buf, size, "%s0x%02X", "add a,", cpu.operand); break;
	case 0xC7: snprintf(buf, size, "%s0x%02X", "rst ", 0); break;
	case 0xC8: snprintf(buf, size, "%s", "ret z"); break;
	case 0xC9: snprintf(buf, size, "%s", "ret"); break;
	case 0xCA: snprintf(buf, size, "%s0x%04X", "jp z,", cpu.operand); break;

	case 0xCB:
		switch (cpu.cb_op) {
		case 0x00: snprintf(buf, size, "%s", "rlc b"); break;
		case 0x01: snprintf(buf, size, "%s", "rlc c"); break;
		case 0x02: snprintf(buf, size, "%s", "rlc d"); break;
		case 0x03: snprintf(buf, size, "%s", "rlc e"); break;
		case 0x04: snprintf(buf, size, "%s", "rlc h"); break;
		case 0x05: snprintf(buf, size, "%s", "rlc l"); break;
		case 0x06: snprintf(buf, size, "%s", "rlc (hl)"); break;
		case 0x07: snprintf(buf, size, "%s", "rlc a"); break;

		case 0x08: snprintf(buf, size, "%s", "rrc b"); break;
		case 0x09: snprintf(buf, size, "%s", "rrc c"); break;
		case 0x0A: snprintf(buf, size, "%s", "rrc d"); break;
		case 0x0B: snprintf(buf, size, "%s", "rrc e"); break;
		case 0x0C: snprintf(buf, size, "%s", "rrc h"); break;
		case 0x0D: snprintf(buf, size, "%s", "rrc l"); break;
		case 0x0E: snprintf(buf, size, "%s", "rrc (hl)"); break;
		case 0x0F: snprintf(buf, size, "%s", "rrc a"); break;

		case 0x10: snprintf(buf, size, "%s", "rl b"); break;
		case 0x11: snprintf(buf, size, "%s", "rl c"); break;
		case 0x12: snprintf(buf, size, "%s", "rl d"); break;
		case 0x13: snprintf(buf, size, "%s", "rl e"); break;
		case 0x14: snprintf(buf, size, "%s", "rl h"); break;
		case 0x15: snprintf(buf, size, "%s", "rl l"); break;
		case 0x16: snprintf(buf, size, "%s", "rl (hl)"); break;
		case 0x17: snprintf(buf, size, "%s", "rl a"); break;

		case 0x18: snprintf(buf, size, "%s", "rr b"); break;
		case 0x19: snprintf(buf, size, "%s", "rr c"); break;
		case 0x1A: snprintf(buf, size, "%s", "rr d"); break;
		case 0x1B: snprintf(buf, size, "%s", "rr e"); break;
		case 0x1C: snprintf(buf, size, "%s", "rr h"); break;
		case 0x1D: snprintf(buf, size, "%s", "rr l"); break;
		case 0x1E: snprintf(buf, size, "%s", "rr (hl)"); break;
		case 0x1F: snprintf(buf, size, "%s", "rr a"); break;

		case 0x20: snprintf(buf, size, "%s", "sla b"); break;
		case 0x21: snprintf(buf, size, "%s", "sla c"); break;
		case 0x22: snprintf(buf, size, "%s", "sla d"); break;
		case 0x23: snprintf(buf, size, "%s", "sla e"); break;
		case 0x24: snprintf(buf, size, "%s", "sla h"); break;
		case 0x25: snprintf(buf, size, "%s", "sla l"); break;
		case 0x26: snprintf(buf, size, "%s", "sla (hl)"); break;
		case 0x27: snprintf(buf, size, "%s", "sla a"); break;

		case 0x28: snprintf(buf, size, "%s", "sra b"); break;
		case 0x29: snprintf(buf, size, "%s", "sra c"); break;
		case 0x2A: snprintf(buf, size, "%s", "sra d"); break;
		case 0x2B: snprintf(buf, size, "%s", "sra e"); break;
		case 0x2C: snprintf(buf, size, "%s", "sra h"); break;
		case 0x2D: snprintf(buf, size, "%s", "sra l"); break;
		case 0x2E: snprintf(buf, size, "%s", "sra (hl)"); break;
		case 0x2F: snprintf(buf, size, "%s", "sra a"); break;

		case 0x30: snprintf(buf, size, "%s", "swap b"); break;
		case 0x31: snprintf(buf, size, "%s", "swap c"); break;
		case 0x32: snprintf(buf, size, "%s", "swap d"); break;
		case 0x33: snprintf(buf, size, "%s", "swap e"); break;
		case 0x34: snprintf(buf, size, "%s", "swap h"); break;
		case 0x35: snprintf(buf, size, "%s", "swap l"); break;
		case 0x36: snprintf(buf, size, "%s", "swap (hl)"); break;
		case 0x37: snprintf(buf, size, "%s", "swap a"); break;

		case 0x38: snprintf(buf, size, "%s", "srl b"); break;
		case 0x39: snprintf(buf, size, "%s", "srl c"); break;
		case 0x3A: snprintf(buf, size, "%s", "srl d"); break;
		case 0x3B: snprintf(buf, size, "%s", "srl e"); break;
		case 0x3C: snprintf(buf, size, "%s", "srl h"); break;
		case 0x3D: snprintf(buf, size, "%s", "srl l"); break;
		case 0x3E: snprintf(buf, size, "%s", "srl (hl)"); break;
		case 0x3F: snprintf(buf, size, "%s", "srl a"); break;

		case 0x40: snprintf(buf, size, "%s", "bit 0,b"); break;
		case 0x41: snprintf(buf, size, "%s", "bit 0,c"); break;
		case 0x42: snprintf(buf, size, "%s", "bit 0,d"); break;
		case 0x43: snprintf(buf, size, "%s", "bit 0,e"); break;
		case 0x44: snprintf(buf, size, "%s", "bit 0,h"); break;
		case 0x45: snprintf(buf, size, "%s", "bit 0,l"); break;
		case 0x46: snprintf(buf, size, "%s", "bit 0,(hl)"); break;
		case 0x47: snprintf(buf, size, "%s", "bit 0,a"); break;

		case 0x48: snprintf(buf, size, "%s", "bit 1,b"); break;
		case 0x49: snprintf(buf, size, "%s", "bit 1,c"); break;
		case 0x4A: snprintf(buf, size, "%s", "bit 1,d"); break;
		case 0x4B: snprintf(buf, size, "%s", "bit 1,e"); break;
		case 0x4C: snprintf(buf, size, "%s", "bit 1,h"); break;
		case 0x4D: snprintf(buf, size, "%s", "bit 1,l"); break;
		case 0x4E: snprintf(buf, size, "%s", "bit 1,(hl)"); break;
		case 0x4F: snprintf(buf, size, "%s", "bit 1,a"); break;

		case 0x50: snprintf(buf, size, "%s", "bit 2,b"); break;
		case 0x51: snprintf(buf, size, "%s", "bit 2,c"); break;
		case 0x52: snprintf(buf, size, "%s", "bit 2,d"); break;
		case 0x53: snprintf(buf, size, "%s", "bit 2,e"); break;
		case 0x54: snprintf(buf, size, "%s", "bit 2,h"); break;
		case 0x55: snprintf(buf, size, "%s", "bit 2,l"); break;
		case 0x56: snprintf(buf, size, "%s", "bit 2,(hl)"); break;
		case 0x57: snprintf(buf, size, "%s", "bit 2,a"); break;

		case 0x58: snprintf(buf, size, "%s", "bit 3,b"); break;
		case 0x59: snprintf(buf, size, "%s", "bit 3,c"); break;
		case 0x5A: snprintf(buf, size, "%s", "bit 3,d"); break;
		case 0x5B: snprintf(buf, size, "%s", "bit 3,e"); break;
		case 0x5C: snprintf(buf, size, "%s", "bit 3,h"); break;
		case 0x5D: snprintf(buf, size, "%s", "bit 3,l"); break;
		case 0x5E: snprintf(buf, size, "%s", "bit 3,(hl)"); break;
		case 0x5F: snprintf(buf, size, "%s", "bit 3,a"); break;

		case 0x60: snprintf(buf, size, "%s", "bit 4,b"); break;
		case 0x61: snprintf(buf, size, "%s", "bit 4,c"); break;
		case 0x62: snprintf(buf, size, "%s", "bit 4,d"); break;
		case 0x63: snprintf(buf, size, "%s", "bit 4,e"); break;
		case 0x64: snprintf(buf, size, "%s", "bit 4,h"); break;
		case 0x65: snprintf(buf, size, "%s", "bit 4,l"); break;
		case 0x66: snprintf(buf, size, "%s", "bit 4,(hl)"); break;
		case 0x67: snprintf(buf, size, "%s", "bit 4,a"); break;

		case 0x68: snprintf(buf, size, "%s", "bit 5,b"); break;
		case 0x69: snprintf(buf, size, "%s", "bit 5,c"); break;
		case 0x6A: snprintf(buf, size, "%s", "bit 5,d"); break;
		case 0x6B: snprintf(buf, size, "%s", "bit 5,e"); break;
		case 0x6C: snprintf(buf, size, "%s", "bit 5,h"); break;
		case 0x6D: snprintf(buf, size, "%s", "bit 5,l"); break;
		case 0x6E: snprintf(buf, size, "%s", "bit 5,(hl)"); break;
		case 0x6F: snprintf(buf, size, "%s", "bit 5,a"); break;

		case 0x70: snprintf(buf, size, "%s", "bit 6,b"); break;
		case 0x71: snprintf(buf, size, "%s", "bit 6,c"); break;
		case 0x72: snprintf(buf, size, "%s", "bit 6,d"); break;
		case 0x73: snprintf(buf, size, "%s", "bit 6,e"); break;
		case 0x74: snprintf(buf, size, "%s", "bit 6,h"); break;
		case 0x75: snprintf(buf, size, "%s", "bit 6,l"); break;
		case 0x76: snprintf(buf, size, "%s", "bit 6,(hl)"); break;
		case 0x77: snprintf(buf, size, "%s", "bit 6,a"); break;

		case 0x78: snprintf(buf, size, "%s", "bit 7,b"); break;
		case 0x79: snprintf(buf, size, "%s", "bit 7,c"); break;
		case 0x7A: snprintf(buf, size, "%s", "bit 7,d"); break;
		case 0x7B: snprintf(buf, size, "%s", "bit 7,e"); break;
		case 0x7C: snprintf(buf, size, "%s", "bit 7,h"); break;
		case 0x7D: snprintf(buf, size, "%s", "bit 7,l"); break;
		case 0x7E: snprintf(buf, size, "%s", "bit 7,(hl)"); break;
		case 0x7F: snprintf(buf, size, "%s", "bit 7,a"); break;

		case 0x80: snprintf(buf, size, "%s", "res 0,b"); break;
		case 0x81: snprintf(buf, size, "%s", "res 0,c"); break;
		case 0x82: snprintf(buf, size, "%s", "res 0,d"); break;
		case 0x83: snprintf(buf, size, "%s", "res 0,e"); break;
		case 0x84: snprintf(buf, size, "%s", "res 0,h"); break;
		case 0x85: snprintf(buf, size, "%s", "res 0,l"); break;
		case 0x86: snprintf(buf, size, "%s", "res 0,(hl)"); break;
		case 0x87: snprintf(buf, size, "%s", "res 0,a"); break;

		case 0x88: snprintf(buf, size, "%s", "res 1,b"); break;
		case 0x89: snprintf(buf, size, "%s", "res 1,c"); break;
		case 0x8A: snprintf(buf, size, "%s", "res 1,d"); break;
		case 0x8B: snprintf(buf, size, "%s", "res 1,e"); break;
		case 0x8C: snprintf(buf, size, "%s", "res 1,h"); break;
		case 0x8D: snprintf(buf, size, "%s", "res 1,l"); break;
		case 0x8E: snprintf(buf, size, "%s", "res 1,(hl)"); break;
		case 0x8F: snprintf(buf, size, "%s", "res 1,a"); break;

		case 0x90: snprintf(buf, size, "%s", "res 2,b"); break;
		case 0x91: snprintf(buf, size, "%s", "res 2,c"); break;
		case 0x92: snprintf(buf, size, "%s", "res 2,d"); break;
		case 0x93: snprintf(buf, size, "%s", "res 2,e"); break;
		case 0x94: snprintf(buf, size, "%s", "res 2,h"); break;
		case 0x95: snprintf(buf, size, "%s", "res 2,l"); break;
		case 0x96: snprintf(buf, size, "%s", "res 2,(hl)"); break;
		case 0x97: snprintf(buf, size, "%s", "res 2,a"); break;

		case 0x98: snprintf(buf, size, "%s", "res 3,b"); break;
		case 0x99: snprintf(buf, size, "%s", "res 3,c"); break;
		case 0x9A: snprintf(buf, size, "%s", "res 3,d"); break;
		case 0x9B: snprintf(buf, size, "%s", "res 3,e"); break;
		case 0x9C: snprintf(buf, size, "%s", "res 3,h"); break;
		case 0x9D: snprintf(buf, size, "%s", "res 3,l"); break;
		case 0x9E: snprintf(buf, size, "%s", "res 3,(hl)"); break;
		case 0x9F: snprintf(buf, size, "%s", "res 3,a"); break;

		case 0xA0: snprintf(buf, size, "%s", "res 4,b"); break;
		case 0xA1: snprintf(buf, size, "%s", "res 4,c"); break;
		case 0xA2: snprintf(buf, size, "%s", "res 4,d"); break;
		case 0xA3: snprintf(buf, size, "%s", "res 4,e"); break;
		case 0xA4: snprintf(buf, size, "%s", "res 4,h"); break;
		case 0xA5: snprintf(buf, size, "%s", "res 4,l"); break;
		case 0xA6: snprintf(buf, size, "%s", "res 4,(hl)"); break;
		case 0xA7: snprintf(buf, size, "%s", "res 4,a"); break;

		case 0xA8: snprintf(buf, size, "%s", "res 5,b"); break;
		case 0xA9: snprintf(buf, size, "%s", "res 5,c"); break;
		case 0xAA: snprintf(buf, size, "%s", "res 5,d"); break;
		case 0xAB: snprintf(buf, size, "%s", "res 5,e"); break;
		case 0xAC: snprintf(buf, size, "%s", "res 5,h"); break;
		case 0xAD: snprintf(buf, size, "%s", "res 5,l"); break;
		case 0xAE: snprintf(buf, size, "%s", "res 5,(hl)"); break;
		case 0xAF: snprintf(buf, size, "%s", "res 5,a"); break;

		case 0xB0: snprintf(buf, size, "%s", "res 6,b"); break;
		case 0xB1: snprintf(buf, size, "%s", "res 6,c"); break;
		case 0xB2: snprintf(buf, size, "%s", "res 6,d"); break;
		case 0xB3: snprintf(buf, size, "%s", "res 6,e"); break;
		case 0xB4: snprintf(buf, size, "%s", "res 6,h"); break;
		case 0xB5: snprintf(buf, size, "%s", "res 6,l"); break;
		case 0xB6: snprintf(buf, size, "%s", "res 6,(hl)"); break;
		case 0xB7: snprintf(buf, size, "%s", "res 6,a"); break;

		case 0xB8: snprintf(buf, size, "%s", "res 7,b"); break;
		case 0xB9: snprintf(buf, size, "%s", "res 7,c"); break;
		case 0xBA: snprintf(buf, size, "%s", "res 7,d"); break;
		case 0xBB: snprintf(buf, size, "%s", "res 7,e"); break;
		case 0xBC: snprintf(buf, size, "%s", "res 7,h"); break;
		case 0xBD: snprintf(buf, size, "%s", "res 7,l"); break;
		case 0xBE: snprintf(buf, size, "%s", "res 7,(hl)"); break;
		case 0xBF: snprintf(buf, size, "%s", "res 7,a"); break;

		case 0xC0: snprintf(buf, size, "%s", "set 0,b"); break;
		case 0xC1: snprintf(buf, size, "%s", "set 0,c"); break;
		case 0xC2: snprintf(buf, size, "%s", "set 0,d"); break;
		case 0xC3: snprintf(buf, size, "%s", "set 0,e"); break;
		case 0xC4: snprintf(buf, size, "%s", "set 0,h"); break;
		case 0xC5: snprintf(buf, size, "%s", "set 0,l"); break;
		case 0xC6: snprintf(buf, size, "%s", "set 0,(hl)"); break;
		case 0xC7: snprintf(buf, size, "%s", "set 0,a"); break;

		case 0xC8: snprintf(buf, size, "%s", "set 1,b"); break;
		case 0xC9: snprintf(buf, size, "%s", "set 1,c"); break;
		case 0xCA: snprintf(buf, size, "%s", "set 1,d"); break;
		case 0xCB: snprintf(buf, size, "%s", "set 1,e"); break;
		case 0xCC: snprintf(buf, size, "%s", "set 1,h"); break;
		case 0xCD: snprintf(buf, size, "%s", "set 1,l"); break;
		case 0xCE: snprintf(buf, size, "%s", "set 1,(hl)"); break;
		case 0xCF: snprintf(buf, size, "%s", "set 1,a"); break;

		case 0xD0: snprintf(buf, size, "%s", "set 2,b"); break;
		case 0xD1: snprintf(buf, size, "%s", "set 2,c"); break;
		case 0xD2: snprintf(buf, size, "%s", "set 2,d"); break;
		case 0xD3: snprintf(buf, size, "%s", "set 2,e"); break;
		case 0xD4: snprintf(buf, size, "%s", "set 2,h"); break;
		case 0xD5: snprintf(buf, size, "%s", "set 2,l"); break;
		case 0xD6: snprintf(buf, size, "%s", "set 2,(hl)"); break;
		case 0xD7: snprintf(buf, size, "%s", "set 2,a"); break;

		case 0xD8: snprintf(buf, size, "%s", "set 3,b"); break;
		case 0xD9: snprintf(buf, size, "%s", "set 3,c"); break;
		case 0xDA: snprintf(buf, size, "%s", "set 3,d"); break;
		case 0xDB: snprintf(buf, size, "%s", "set 3,e"); break;
		case 0xDC: snprintf(buf, size, "%s", "set 3,h"); break;
		case 0xDD: snprintf(buf, size, "%s", "set 3,l"); break;
		case 0xDE: snprintf(buf, size, "%s", "set 3,(hl)"); break;
		case 0xDF: snprintf(buf, size, "%s", "set 3,a"); break;

		case 0xE0: snprintf(buf, size, "%s", "set 4,b"); break;
		case 0xE1: snprintf(buf, size, "%s", "set 4,c"); break;
		case 0xE2: snprintf(buf, size, "%s", "set 4,d"); break;
		case 0xE3: snprintf(buf, size, "%s", "set 4,e"); break;
		case 0xE4: snprintf(buf, size, "%s", "set 4,h"); break;
		case 0xE5: snprintf(buf, size, "%s", "set 4,l"); break;
		case 0xE6: snprintf(buf, size, "%s", "set 4,(hl)"); break;
		case 0xE7: snprintf(buf, size, "%s", "set 4,a"); break;

		case 0xE8: snprintf(buf, size, "%s", "set 5,b"); break;
		case 0xE9: snprintf(buf, size, "%s", "set 5,c"); break;
		case 0xEA: snprintf(buf, size, "%s", "set 5,d"); break;
		case 0xEB: snprintf(buf, size, "%s", "set 5,e"); break;
		case 0xEC: snprintf(buf, size, "%s", "set 5,h"); break;
		case 0xED: snprintf(buf, size, "%s", "set 5,l"); break;
		case 0xEE: snprintf(buf, size, "%s", "set 5,(hl)"); break;
		case 0xEF: snprintf(buf, size, "%s", "set 5,a"); break;

		case 0xF0: snprintf(buf, size, "%s", "set 6,b"); break;
		case 0xF1: snprintf(buf, size, "%s", "set 6,c"); break;
		case 0xF2: snprintf(buf, size, "%s", "set 6,d"); break;
		case 0xF3: snprintf(buf, size, "%s", "set 6,e"); break;
		case 0xF4: snprintf(buf, size, "%s", "set 6,h"); break;
		case 0xF5: snprintf(buf, size, "%s", "set 6,l"); break;
		case 0xF6: snprintf(buf, size, "%s", "set 6,(hl)"); break;
		case 0xF7: snprintf(buf, size, "%s", "set 6,a"); break;

		case 0xF8: snprintf(buf, size, "%s", "set 7,b"); break;
		case 0xF9: snprintf(buf, size, "%s", "set 7,c"); break;
		case 0xFA: snprintf(buf, size, "%s", "set 7,d"); break;
		case 0xFB: snprintf(buf, size, "%s", "set 7,e"); break;
		case 0xFC: snprintf(buf, size, "%s", "set 7,h"); break;
		case 0xFD: snprintf(buf, size, "%s", "set 7,l"); break;
		case 0xFE: snprintf(buf, size, "%s", "set 7,(hl)"); break;
		case 0xFF: snprintf(buf, size, "%s", "set 7,a"); break;
		}
		break;

	case 0xCC: snprintf(buf, size, "%s0x%04X", "call z,", cpu.operand); break;
	case 0xCD: snprintf(buf, size, "%s0x%04X", "call ", cpu.operand); break;
	case 0xCE: snprintf(buf, size, "%s0x%02X", "adc a,", cpu.operand); break;
	case 0xCF: snprintf(buf, size, "%s0x%02X", "rst ", 0x08); break;

	case 0xD0: snprintf(buf, size, "%s", "ret nc"); break;
	case 0xD1: snprintf(buf, size, "%s", "pop de"); break;
	case 0xD2: snprintf(buf, size, "%s0x%04X", "jp nc,", cpu.operand); break;
	case 0xD3: snprintf(buf, size, "%s", "nop"); break;
	case 0xD4: snprintf(buf, size, "%s0x%04X", "call nc,", cpu.operand); break;
	case 0xD5: snprintf(buf, size, "%s", "push de"); break;
	case 0xD6: snprintf(buf, size, "%s0x%02X", "sub a,", cpu.operand); break;
	case 0xD7: snprintf(buf, size, "%s0x%02X", "rst ", 0x10); break;
	case 0xD8: snprintf(buf, size, "%s", "ret c"); break;
	case 0xD9: snprintf(buf, size, "%s", "reti"); break;
	case 0xDA: snprintf(buf, size, "%s0x%04X", "jp c,", cpu.operand); break;
	case 0xDB: snprintf(buf, size, "%s", "nop"); break;
	case 0xDC: snprintf(buf, size, "%s0x%04X", "call c,", cpu.operand); break;
	case 0xDD: snprintf(buf, size, "%s", "nop"); break;
	case 0xDE: snprintf(buf, size, "%s0x%02X", "sbc a,", cpu.operand); break;
	case 0xDF: snprintf(buf, size, "%s0x%02X", "rst ", 0x18); break;

	case 0xE0: snprintf(buf, size, "%s0x%02X%s", "ldh (", cpu.operand, "),a"); break; //TODO check
	case 0xE1: snprintf(buf, size, "%s", "pop hl"); break;
	case 0xE2: snprintf(buf, size, "%s", "ld (c),a"); break;
	case 0xE3: snprintf(buf, size, "%s", "nop"); break;
	case 0xE4: snprintf(buf, size, "%s", "nop"); break;
	case 0xE5: snprintf(buf, size, "%s", "push hl"); break;
	case 0xE6: snprintf(buf, size, "%s0x%02X", "and ", cpu.operand); break;
	case 0xE7: snprintf(buf, size, "%s0x%02X", "rst ", 0x20); break;
	case 0xE8: snprintf(buf, size, "%s0x%02X", "add sp,", cpu.operand); break;
	case 0xE9: snprintf(buf, size, "%s", "jp (hl)"); break;
	case 0xEA: snprintf(buf, size, "%s0x%04X%s", "ld (", cpu.operand, "),a"); break;
	case 0xEB: snprintf(buf, size, "%s", "nop"); break;
	case 0xEC: snprintf(buf, size, "%s", "nop"); break;
	case 0xED: snprintf(buf, size, "%s", "nop"); break;
	case 0xEE: snprintf(buf, size, "%s0x%02X", "xor ", cpu.operand); break;
	case 0xEF: snprintf(buf, size, "%s0x%02X", "rst ", 0x28); break;

	case 0xF0: snprintf(buf, size, "%s0x%02X%s", "ldh a,(", cpu.operand, ")"); break;
	case 0xF1: snprintf(buf, size, "%s", "pop af"); break;
	case 0xF2: snprintf(buf, size, "%s", "ld a,(c)"); break;
	case 0xF3: snprintf(buf, size, "%s", "di"); break;
	case 0xF4: snprintf(buf, size, "%s", "nop"); break;
	case 0xF5: snprintf(buf, size, "%s", "push af"); break;
	case 0xF6: snprintf(buf, size, "%s0x%02X", "or ", cpu.operand); break;
	case 0xF7: snprintf(buf, size, "%s0x%02X", "rst ", 0x30); break;
	case 0xF8: snprintf(buf, size, "%s0x%02X", "ld hl,sp+", cpu.operand); break;
	case 0xF9: snprintf(buf, size, "%s", "ld sp,hl"); break;
	case 0xFA: snprintf(buf, size, "%s0x%04X%s", "ld a,(", cpu.operand, ")"); break;
	case 0xFB: snprintf(buf, size, "%s", "ei"); break;
	case 0xFC: snprintf(buf, size, "%s", "nop"); break;
	case 0xFD: snprintf(buf, size, "%s", "nop"); break;
	case 0xFE: snprintf(buf, size, "%s0x%02X", "cp ", cpu.operand); break;
	case 0xFF: snprintf(buf, size, "%s0x%02X", "rst ", 0x08); break;
	}
}
