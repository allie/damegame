#include "cpu.h"
#include "mmu.h"
#include "debugger.h"
#include <string.h>

CPU cpu;
CPU_history cpu_history;
extern MMU mmu;

// Helper functions
static void push(WORD val) {
	REG_SP -= 2;
	MMU_write_16(REG_SP, val);
}

static WORD pop(void) {
	REG_SP += 2;
	return MMU_read_16(REG_SP - 2);
}

static BYTE add_8_8(BYTE a, BYTE b) {
	BYTE ret = a + b;

	FLAG_C = (0xFF - a < b);
	FLAG_H = (0x0F - (a & 0x0F) < (b & 0x0F));
	FLAG_Z = (ret == 0);
	FLAG_N = 0;

	return ret;
}

static WORD add_16_16(WORD a, WORD b) {
	WORD ret = a + b;

	FLAG_C = (0xFFFF - a < b);
	FLAG_H = (0x0FFF - (a & 0x0FFF) < (b & 0x0FFF));
	FLAG_N = 0;

	return ret;
}

static WORD add_16_8(WORD a, BYTE b) {
	WORD ret = a + (WORD)b;

	FLAG_C = (0xFF - (a & 0x00ff) < b);
	FLAG_H = (0x0F - (a & 0x0F) < (b & 0x0F));
	FLAG_Z = 0;
	FLAG_N = 0;

	return ret;
}

static BYTE adc(BYTE a, BYTE b) {
	BYTE tmp, ret;
	BYTE h = 0;
	BYTE c = 0;

	tmp = a + FLAG_C;

	if (0x0F - (a & 0x0F) < (FLAG_C & 0x0F))
	h = 1;

	if (0xFF - a < FLAG_C)
	c = 1;

	ret = tmp + b;

	if (0x0F - (tmp & 0x0F) < (b & 0x0F))
	h = 1;

	if (0xFF - tmp < b)
	c = 1;

	FLAG_H = h;
	FLAG_C = c;
	FLAG_Z = (ret == 0);
	FLAG_N = 0;

	return ret;
}

static BYTE sub(BYTE a, BYTE b) {
	BYTE ret;

	FLAG_C = (a < b);
	FLAG_H = ((a & 0x0F) < (b & 0x0F));

	ret = a - b;

	FLAG_Z = (ret == 0);
	FLAG_N = 1;

	return ret;
}

static BYTE sbc(BYTE a, BYTE b) {
	BYTE tmp, ret;
	BYTE h = 0;
	BYTE c = 0;

	tmp = a - FLAG_C;

	if (a < FLAG_C || tmp < b)
	c = 1;

	if ((a & 0x0F) < (FLAG_C & 0x0F) || (tmp & 0x0F) < (b & 0x0F))
	h = 1;

	ret = tmp - b;

	FLAG_H = h;
	FLAG_C = c;
	FLAG_Z = (ret == 0);
	FLAG_N = 1;

	return ret;
}

static BYTE inc_8(BYTE a) {
	a++;

	FLAG_H = ((a & 0x0F) == 0);
	FLAG_Z = (a == 0);
	FLAG_N = 0;

	return a;
}

static WORD inc_16(WORD a) {
	return a + 1;
}

static BYTE dec_8(BYTE a) {
	a--;

	FLAG_H = ((a & 0x0F) == 0x0F);
	FLAG_Z = (a == 0);
	FLAG_N = 1;

	return a;
}

static WORD dec_16(WORD a) {
	return a - 1;
}

static BYTE and(BYTE a, BYTE b) {
	BYTE ret = a & b;

	FLAG_N = 0;
	FLAG_H = 1;
	FLAG_C = 0;
	FLAG_Z = (ret == 0);

	return ret;
}

static BYTE or(BYTE a, BYTE b) {
	BYTE ret = a | b;

	FLAG_N = 0;
	FLAG_H = 0;
	FLAG_C = 0;
	FLAG_Z = (ret == 0);

	return ret;
}

static BYTE xor(BYTE a, BYTE b) {
	BYTE ret = a ^ b;

	FLAG_N = 0;
	FLAG_H = 0;
	FLAG_C = 0;
	FLAG_Z = (ret == 0);

	return ret;
}

static BYTE swap(BYTE a) {
	BYTE tmp = a & 0x0F;

	a >>= 4;
	a |= (tmp << 4);

	FLAG_Z = (a == 0);
	FLAG_N = 0;
	FLAG_H = 0;
	FLAG_C = 0;

	return a;
}

static BYTE rlc(BYTE a) {
	FLAG_C = (a & 0x80) >> 7;
	a = (a << 1) + FLAG_C;

	FLAG_Z = (a == 0);
	FLAG_N = 0;
	FLAG_H = 0;

	return a;
}

static BYTE rl(BYTE a) {
	BYTE tmp = FLAG_C;
	FLAG_C = (a & 0x80) >> 7;
	a = (a << 1) + tmp;

	FLAG_Z = (a == 0);
	FLAG_N = 0;
	FLAG_H = 0;

	return a;
}

static BYTE rrc(BYTE a) {
	FLAG_C = a & 0x01;
	a = (a >> 1) + (FLAG_C << 7);

	FLAG_Z = (a == 0);
	FLAG_N = 0;
	FLAG_N = 0;

	return a;
}

static BYTE rr(BYTE a) {
	BYTE tmp = FLAG_C;
	FLAG_C = a & 0x01;
	a = (a >> 1) + (tmp << 7);

	FLAG_Z = (a == 0);
	FLAG_N = 0;
	FLAG_H = 0;

	return a;
}

static BYTE sla(BYTE a) {
	FLAG_C = (a & 0x80) >> 7;
	a <<= 1;

	FLAG_Z = (a == 0);
	FLAG_N = 0;
	FLAG_H = 0;

	return a;
}

static BYTE sra(BYTE a) {
	FLAG_C = a & 0x01;
	a >>= 1;
	a |= ((a & 0x40) << 1);

	FLAG_Z = (a == 0);
	FLAG_N = 0;
	FLAG_H = 0;

	return a;
}

static BYTE srl(BYTE a) {
	FLAG_C = a & 0x01;
	a >>= 1;

	FLAG_Z = (a == 0);
	FLAG_N = 0;
	FLAG_H = 0;

	return a;
}

static void bit(BYTE reg, BYTE b) {
	FLAG_Z = ((reg & (0x01 << b)) == 0);
	FLAG_N = 0;
	FLAG_H = 1;
}

static BYTE set(BYTE reg, BYTE b) {
	return reg |= (1 << b);
}

static BYTE res(BYTE reg, BYTE b) {
	return reg &= ~(1 << b);
}

static void jr(BYTE a) {
	if ((a & 0x80) == 0x80) {
		a--;
		a = ~a;
		REG_PC -= a;
	} else {
		REG_PC += a;
	}
}

static void call(WORD addr) {
	push(REG_PC + 2);
	REG_PC = addr;
}

static void rst(BYTE a) {
	push(REG_PC);
	REG_PC = 0x0000 + a;
}

static void ret(void) {
	REG_PC = pop();
}

// 8-bit loads
// LD r <- s
static void LDbb(void) { REG_B = REG_B; }
static void LDbc(void) { REG_B = REG_C; }
static void LDbd(void) { REG_B = REG_D; }
static void LDbe(void) { REG_B = REG_E; }
static void LDbh(void) { REG_B = REG_H; }
static void LDbl(void) { REG_B = REG_L; }
static void LDba(void) { REG_B = REG_A; }
static void LDcb(void) { REG_C = REG_B; }
static void LDcc(void) { REG_C = REG_C; }
static void LDcd(void) { REG_C = REG_D; }
static void LDce(void) { REG_C = REG_E; }
static void LDch(void) { REG_C = REG_H; }
static void LDcl(void) { REG_C = REG_L; }
static void LDca(void) { REG_C = REG_A; }
static void LDdb(void) { REG_D = REG_B; }
static void LDdc(void) { REG_D = REG_C; }
static void LDdd(void) { REG_D = REG_D; }
static void LDde(void) { REG_D = REG_E; }
static void LDdh(void) { REG_D = REG_H; }
static void LDdl(void) { REG_D = REG_L; }
static void LDda(void) { REG_D = REG_A; }
static void LDeb(void) { REG_E = REG_B; }
static void LDec(void) { REG_E = REG_C; }
static void LDed(void) { REG_E = REG_D; }
static void LDee(void) { REG_E = REG_E; }
static void LDeh(void) { REG_E = REG_H; }
static void LDel(void) { REG_E = REG_L; }
static void LDea(void) { REG_E = REG_A; }
static void LDhb(void) { REG_H = REG_B; }
static void LDhc(void) { REG_H = REG_C; }
static void LDhd(void) { REG_H = REG_D; }
static void LDhe(void) { REG_H = REG_E; }
static void LDhh(void) { REG_H = REG_H; }
static void LDhl(void) { REG_H = REG_L; }
static void LDha(void) { REG_H = REG_A; }
static void LDlb(void) { REG_L = REG_B; }
static void LDlc(void) { REG_L = REG_C; }
static void LDld(void) { REG_L = REG_D; }
static void LDle(void) { REG_L = REG_E; }
static void LDlh(void) { REG_L = REG_H; }
static void LDll(void) { REG_L = REG_L; }
static void LDla(void) { REG_L = REG_A; }
static void LDab(void) { REG_A = REG_B; }
static void LDac(void) { REG_A = REG_C; }
static void LDad(void) { REG_A = REG_D; }
static void LDae(void) { REG_A = REG_E; }
static void LDah(void) { REG_A = REG_H; }
static void LDal(void) { REG_A = REG_L; }
static void LDaa(void) { REG_A = REG_A; }
static void LDbn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_B = cpu.operand; }
static void LDcn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_C = cpu.operand; }
static void LDdn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_D = cpu.operand; }
static void LDen(void) { cpu.operand = MMU_read_8(REG_PC++); REG_E = cpu.operand; }
static void LDhn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_H = cpu.operand; }
static void LDln(void) { cpu.operand = MMU_read_8(REG_PC++); REG_L = cpu.operand; }
static void LDan(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = cpu.operand; }
static void LDbmHL(void) { REG_B = MMU_read_8(REG_HL); }
static void LDcmHL(void) { REG_C = MMU_read_8(REG_HL); }
static void LDdmHL(void) { REG_D = MMU_read_8(REG_HL); }
static void LDemHL(void) { REG_E = MMU_read_8(REG_HL); }
static void LDhmHL(void) { REG_H = MMU_read_8(REG_HL); }
static void LDlmHL(void) { REG_L = MMU_read_8(REG_HL); }
// LD d <- r
static void LDmHLb(void) { MMU_write_8(REG_HL, REG_B); }
static void LDmHLc(void) { MMU_write_8(REG_HL, REG_C); }
static void LDmHLd(void) { MMU_write_8(REG_HL, REG_D); }
static void LDmHLe(void) { MMU_write_8(REG_HL, REG_E); }
static void LDmHLh(void) { MMU_write_8(REG_HL, REG_H); }
static void LDmHLl(void) { MMU_write_8(REG_HL, REG_L); }
// LD d <- n
static void LDmHLn(void) { cpu.operand = MMU_read_8(REG_PC++); MMU_write_8(REG_HL, cpu.operand); }
// LD A <- (ss)
static void LDamBC(void) { cpu.operand = MMU_read_8(REG_BC); REG_A = cpu.operand; }
static void LDamDE(void) { cpu.operand = MMU_read_8(REG_DE); REG_A = cpu.operand; }
static void LDamHL(void) { cpu.operand = MMU_read_8(REG_HL); REG_A = cpu.operand; }
static void LDamnn(void) { cpu.operand = MMU_read_16(REG_PC); REG_A = MMU_read_8(cpu.operand); REG_PC += 2; }
// LD (dd) <- A
static void LDmBCa(void) { MMU_write_8(MMU_read_16(REG_BC), REG_A); }
static void LDmDEa(void) { MMU_write_8(MMU_read_16(REG_DE), REG_A); }
static void LDmHLa(void) { MMU_write_8(MMU_read_16(REG_HL), REG_A); }
static void LDmnna(void) { MMU_write_8(MMU_read_16(REG_PC), REG_A); REG_PC += 2; }
// LD A <- (C)
static void LDamc(void) { REG_A = MMU_read_8(REG_C); }
// LD (C) <- A
static void LDmca(void) { MMU_write_8(MMU_read_8(REG_C), REG_A); }
// LDD A <- (HL)
static void LDDamHL(void) { REG_A = MMU_read_8(REG_HL--); }
// LDD (HL) <- A
static void LDDmHLa(void) { MMU_write_8(REG_HL--, REG_A); }
// LDI A <- (HL)
static void LDIamHL(void) { REG_A = MMU_read_8(REG_HL++); }
// LDI (HL) <- A
static void LDImHLa(void) { MMU_write_8(REG_HL++, REG_A); }
// LDH (n) <- A
static void LDHmna(void) { cpu.operand = MMU_read_8(REG_PC++); MMU_write_8(((BYTE)cpu.operand) + 0xFF00, REG_A); }
// LDH A <- (n)
static void LDHamn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = MMU_read_8(0xFF00 + ((BYTE)cpu.operand)); }

// 16-bit loads
// LD dd, nn
static void LDBCnn(void) { MMU_read_16(REG_PC); REG_BC = cpu.operand; REG_PC += 2; }
static void LDDEnn(void) { MMU_read_16(REG_PC); REG_DE = cpu.operand; REG_PC += 2; }
static void LDHLnn(void) { MMU_read_16(REG_PC); REG_HL = cpu.operand; REG_PC += 2; }
static void LDSPnn(void) { MMU_read_16(REG_PC); REG_SP = cpu.operand; REG_PC += 2; }
// LD (nn), SP
static void LDmnnSP(void) { cpu.operand = MMU_read_16(REG_PC); MMU_write_16(cpu.operand, REG_SP); REG_PC += 2; }
// LD SP, HL
static void LDSPHL(void) { REG_SP = REG_HL; }
// LD HL, (SP + e)
static void LDHLSPn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_HL = add_16_8(REG_SP, cpu.operand); }
// PUSH ss
static void PUSHBC(void) { push(REG_BC); }
static void PUSHDE(void) { push(REG_DE); }
static void PUSHHL(void) { push(REG_HL); }
static void PUSHAF(void) { push(REG_AF); }
// POP dd
static void POPBC(void) { REG_BC = pop(); }
static void POPDE(void) { REG_DE = pop(); }
static void POPHL(void) { REG_HL = pop(); }
static void POPAF(void) { REG_AF = pop(); }

// 8-bit ALU
// ADD A, s
static void ADDab(void) { REG_A = add_8_8(REG_A, REG_B); }
static void ADDac(void) { REG_A = add_8_8(REG_A, REG_C); }
static void ADDad(void) { REG_A = add_8_8(REG_A, REG_D); }
static void ADDae(void) { REG_A = add_8_8(REG_A, REG_E); }
static void ADDah(void) { REG_A = add_8_8(REG_A, REG_H); }
static void ADDal(void) { REG_A = add_8_8(REG_A, REG_L); }
static void ADDaa(void) { REG_A = add_8_8(REG_A, REG_A); }
static void ADDan(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = add_8_8(REG_A, cpu.operand); }
static void ADDamHL(void) { REG_A = add_8_8(REG_A, MMU_read_8(REG_HL)); }
// ADC A, s
static void ADCab(void) { REG_A = adc(REG_A, REG_B); }
static void ADCac(void) { REG_A = adc(REG_A, REG_C); }
static void ADCad(void) { REG_A = adc(REG_A, REG_D); }
static void ADCae(void) { REG_A = adc(REG_A, REG_E); }
static void ADCah(void) { REG_A = adc(REG_A, REG_H); }
static void ADCal(void) { REG_A = adc(REG_A, REG_L); }
static void ADCaa(void) { REG_A = adc(REG_A, REG_A); }
static void ADCan(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = adc(REG_A, cpu.operand); }
static void ADCamHL(void) { REG_A = adc(REG_A, MMU_read_8(REG_HL)); }
// SUB s
static void SUBab(void) { REG_A = sub(REG_A, REG_B); }
static void SUBac(void) { REG_A = sub(REG_A, REG_C); }
static void SUBad(void) { REG_A = sub(REG_A, REG_D); }
static void SUBae(void) { REG_A = sub(REG_A, REG_E); }
static void SUBah(void) { REG_A = sub(REG_A, REG_H); }
static void SUBal(void) { REG_A = sub(REG_A, REG_L); }
static void SUBaa(void) { REG_A = sub(REG_A, REG_A); }
static void SUBan(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = sub(REG_A, cpu.operand); }
static void SUBamHL(void) { REG_A = sub(REG_A, MMU_read_8(REG_HL)); }
// SBC A, s
static void SBCab(void) { REG_A = sbc(REG_A, REG_B); }
static void SBCac(void) { REG_A = sbc(REG_A, REG_C); }
static void SBCad(void) { REG_A = sbc(REG_A, REG_D); }
static void SBCae(void) { REG_A = sbc(REG_A, REG_E); }
static void SBCah(void) { REG_A = sbc(REG_A, REG_H); }
static void SBCal(void) { REG_A = sbc(REG_A, REG_L); }
static void SBCaa(void) { REG_A = sbc(REG_A, REG_A); }
static void SBCan(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = sbc(REG_A, cpu.operand); }
static void SBCamHL(void) { REG_A = sbc(REG_A, MMU_read_8(REG_HL)); }
// AND s
static void ANDb(void) { REG_A = and(REG_A, REG_B); }
static void ANDc(void) { REG_A = and(REG_A, REG_C); }
static void ANDd(void) { REG_A = and(REG_A, REG_D); }
static void ANDe(void) { REG_A = and(REG_A, REG_E); }
static void ANDh(void) { REG_A = and(REG_A, REG_H); }
static void ANDl(void) { REG_A = and(REG_A, REG_L); }
static void ANDa(void) { REG_A = and(REG_A, REG_A); }
static void ANDn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = and(REG_A, cpu.operand); }
static void ANDmHL(void) { REG_A = and(REG_A, MMU_read_8(REG_HL)); }
// OR s
static void ORb(void) { REG_A = or(REG_A, REG_B); }
static void ORc(void) { REG_A = or(REG_A, REG_C); }
static void ORd(void) { REG_A = or(REG_A, REG_D); }
static void ORe(void) { REG_A = or(REG_A, REG_E); }
static void ORh(void) { REG_A = or(REG_A, REG_H); }
static void ORl(void) { REG_A = or(REG_A, REG_L); }
static void ORa(void) { REG_A = or(REG_A, REG_A); }
static void ORn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = or(REG_A, cpu.operand); }
static void ORmHL(void) { REG_A = or(REG_A, MMU_read_8(REG_HL)); }
// XOR s
static void XORb(void) { REG_A = xor(REG_A, REG_B); }
static void XORc(void) { REG_A = xor(REG_A, REG_C); }
static void XORd(void) { REG_A = xor(REG_A, REG_D); }
static void XORe(void) { REG_A = xor(REG_A, REG_E); }
static void XORh(void) { REG_A = xor(REG_A, REG_H); }
static void XORl(void) { REG_A = xor(REG_A, REG_L); }
static void XORa(void) { REG_A = xor(REG_A, REG_A); }
static void XORn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_A = xor(REG_A, cpu.operand); }
static void XORmHL(void) { REG_A = xor(REG_A, MMU_read_8(REG_HL)); }
// CP s
static void CPb(void) { sub(REG_A, REG_B); }
static void CPc(void) { sub(REG_A, REG_C); }
static void CPd(void) { sub(REG_A, REG_D); }
static void CPe(void) { sub(REG_A, REG_E); }
static void CPh(void) { sub(REG_A, REG_H); }
static void CPl(void) { sub(REG_A, REG_L); }
static void CPa(void) { sub(REG_A, REG_A); }
static void CPn(void) { cpu.operand = MMU_read_8(REG_PC++); sub(REG_A, cpu.operand); }
static void CPmHL(void) { sub(REG_A, MMU_read_8(REG_HL)); }
// INC s
static void INCb(void) { REG_B = inc_8(REG_B); }
static void INCc(void) { REG_C = inc_8(REG_C); }
static void INCd(void) { REG_D = inc_8(REG_D); }
static void INCe(void) { REG_E = inc_8(REG_E); }
static void INCh(void) { REG_H = inc_8(REG_H); }
static void INCl(void) { REG_L = inc_8(REG_L); }
static void INCa(void) { REG_A = inc_8(REG_A); }
static void INCmHL(void) { MMU_write_8(REG_HL, inc_8(MMU_read_8(REG_HL))); }
// DEC s
static void DECb(void) { REG_B = dec_8(REG_B); }
static void DECc(void) { REG_C = dec_8(REG_C); }
static void DECd(void) { REG_D = dec_8(REG_D); }
static void DECe(void) { REG_E = dec_8(REG_E); }
static void DECh(void) { REG_H = dec_8(REG_H); }
static void DECl(void) { REG_L = dec_8(REG_L); }
static void DECa(void) { REG_A = dec_8(REG_A); }
static void DECmHL(void) { MMU_write_8(REG_HL, dec_8(MMU_read_8(REG_HL))); }

// 16-bit arithmetic
// ADD HL, ss
static void ADDHLBC(void) { REG_HL = add_16_16(REG_HL, REG_BC); }
static void ADDHLDE(void) { REG_HL = add_16_16(REG_HL, REG_DE); }
static void ADDHLHL(void) { REG_HL = add_16_16(REG_HL, REG_HL); }
static void ADDHLSP(void) { REG_HL = add_16_16(REG_HL, REG_SP); }
// ADD SP, e
static void ADDSPn(void) { cpu.operand = MMU_read_8(REG_PC++); REG_SP = add_16_8(REG_SP, cpu.operand); }
// INC ss
static void INCBC(void) { REG_BC = inc_16(REG_BC); }
static void INCDE(void) { REG_DE = inc_16(REG_DE); }
static void INCHL(void) { REG_HL = inc_16(REG_HL); }
static void INCSP(void) { REG_SP = inc_16(REG_SP); }
// DEC ss
static void DECBC(void) { REG_BC = dec_16(REG_BC); }
static void DECDE(void) { REG_DE = dec_16(REG_DE); }
static void DECHL(void) { REG_HL = dec_16(REG_HL); }
static void DECSP(void) { REG_SP = dec_16(REG_SP); }

// Misc
// SWAP s
static void SWAPb(void) { REG_B = swap(REG_B); }
static void SWAPc(void) { REG_C = swap(REG_C); }
static void SWAPd(void) { REG_D = swap(REG_D); }
static void SWAPe(void) { REG_E = swap(REG_E); }
static void SWAPh(void) { REG_H = swap(REG_H); }
static void SWAPl(void) { REG_L = swap(REG_L); }
static void SWAPa(void) { REG_A = swap(REG_A); }
static void SWAPmHL(void) { MMU_write_8(REG_HL, swap(MMU_read_8(REG_HL))); }
// DAA
static void DAA(void) {
	BYTE tmp = REG_A;
	if (!FLAG_N) {
		if (FLAG_H || ((tmp & 0x0F) > 9))
			tmp += 6;
		if (FLAG_C || (tmp > 0x9F))
			tmp += 0x60;
	} else {
		if (FLAG_H)
			tmp = (tmp - 6) & 0xFF;
		if (FLAG_C)
			tmp -= 0x60;
	}

	if (tmp & 0x100)
		FLAG_C = 1;

	FLAG_H = 0;

	tmp &= 0xFF;

	FLAG_Z = (tmp == 0);

	REG_A = tmp;
}
// CPL
static void CPL(void) { REG_A = ~REG_A; FLAG_N = 1; FLAG_H = 1; }
// CCF
static void CCF(void) { FLAG_C = (FLAG_C == 0); FLAG_N = 0; FLAG_H = 0; }
// SCF
static void SCF(void) { FLAG_C = 1; FLAG_N = 0; FLAG_H = 0; }
// NOP
static void NOP(void) { /* No operation */ }
// HALT
static void HALT(void) { cpu.halt = 1; }
// STOP
static void STOP(void) { REG_PC++; cpu.stop = 1; }
// DI
static void DI(void) { cpu.ime = 0; }
// EI
static void EI(void) { cpu.ime = 1; }

// Rotates and shifts
// RLCA
static void RLCA(void) { REG_A = rlc(REG_A); FLAG_Z = 0; }
// RLA
static void RLA(void) { REG_A = rl(REG_A); FLAG_Z = 0; }
// RRCA
static void RRCA(void) { REG_A = rrc(REG_A); FLAG_Z = 0; }
// RRA
static void RRA(void) { REG_A = rr(REG_A); FLAG_Z = 0; }
// RLC s
static void RLCb(void) { REG_B = rlc(REG_B); }
static void RLCc(void) { REG_C = rlc(REG_C); }
static void RLCd(void) { REG_D = rlc(REG_D); }
static void RLCe(void) { REG_E = rlc(REG_E); }
static void RLCh(void) { REG_H = rlc(REG_H); }
static void RLCl(void) { REG_L = rlc(REG_L); }
static void RLCa(void) { REG_A = rlc(REG_A); }
static void RLCmHL(void) { MMU_write_8(REG_HL, rlc(MMU_read_8(REG_HL))); }
// RL s
static void RLb(void) { REG_B = rl(REG_B); }
static void RLc(void) { REG_C = rl(REG_C); }
static void RLd(void) { REG_D = rl(REG_D); }
static void RLe(void) { REG_E = rl(REG_E); }
static void RLh(void) { REG_H = rl(REG_H); }
static void RLl(void) { REG_L = rl(REG_L); }
static void RLa(void) { REG_A = rl(REG_A); }
static void RLmHL(void) { MMU_write_8(REG_HL, rl(MMU_read_8(REG_HL))); }
// RRC s
static void RRCb(void) { REG_B = rrc(REG_B); }
static void RRCc(void) { REG_C = rrc(REG_C); }
static void RRCd(void) { REG_D = rrc(REG_D); }
static void RRCe(void) { REG_E = rrc(REG_E); }
static void RRCh(void) { REG_H = rrc(REG_H); }
static void RRCl(void) { REG_L = rrc(REG_L); }
static void RRCa(void) { REG_A = rrc(REG_A); }
static void RRCmHL(void) { MMU_write_8(REG_HL, rrc(MMU_read_8(REG_HL))); }
// RR s
static void RRb(void) { REG_B = rr(REG_B); }
static void RRc(void) { REG_C = rr(REG_C); }
static void RRd(void) { REG_D = rr(REG_D); }
static void RRe(void) { REG_E = rr(REG_E); }
static void RRh(void) { REG_H = rr(REG_H); }
static void RRl(void) { REG_L = rr(REG_L); }
static void RRa(void) { REG_A = rr(REG_A); }
static void RRmHL(void) { MMU_write_8(REG_HL, rr(MMU_read_8(REG_HL))); }
// SLA s
static void SLAb(void) { REG_B = sla(REG_B); }
static void SLAc(void) { REG_C = sla(REG_C); }
static void SLAd(void) { REG_D = sla(REG_D); }
static void SLAe(void) { REG_E = sla(REG_E); }
static void SLAh(void) { REG_H = sla(REG_H); }
static void SLAl(void) { REG_L = sla(REG_L); }
static void SLAa(void) { REG_A = sla(REG_A); }
static void SLAmHL(void) { MMU_write_8(REG_HL, sla(MMU_read_8(REG_HL))); }
// SRA s
static void SRAb(void) { REG_B = sra(REG_B); }
static void SRAc(void) { REG_C = sra(REG_C); }
static void SRAd(void) { REG_D = sra(REG_D); }
static void SRAe(void) { REG_E = sra(REG_E); }
static void SRAh(void) { REG_H = sra(REG_H); }
static void SRAl(void) { REG_L = sra(REG_L); }
static void SRAa(void) { REG_A = sra(REG_A); }
static void SRAmHL(void) { MMU_write_8(REG_HL, sra(MMU_read_8(REG_HL))); }
// SRL s
static void SRLb(void) { REG_B = srl(REG_B); }
static void SRLc(void) { REG_C = srl(REG_C); }
static void SRLd(void) { REG_D = srl(REG_D); }
static void SRLe(void) { REG_E = srl(REG_E); }
static void SRLh(void) { REG_H = srl(REG_H); }
static void SRLl(void) { REG_L = srl(REG_L); }
static void SRLa(void) { REG_A = srl(REG_A); }
static void SRLmHL(void) { MMU_write_8(REG_HL, srl(MMU_read_8(REG_HL))); }

// Bit manipulation
// BIT b, s
static void BIT0b(void) { bit(REG_B, 0); }
static void BIT0c(void) { bit(REG_C, 0); }
static void BIT0d(void) { bit(REG_D, 0); }
static void BIT0e(void) { bit(REG_E, 0); }
static void BIT0h(void) { bit(REG_H, 0); }
static void BIT0l(void) { bit(REG_L, 0); }
static void BIT0a(void) { bit(REG_A, 0); }
static void BIT0mHL(void) { bit(MMU_read_8(REG_HL), 0); }
static void BIT1b(void) { bit(REG_B, 1); }
static void BIT1c(void) { bit(REG_C, 1); }
static void BIT1d(void) { bit(REG_D, 1); }
static void BIT1e(void) { bit(REG_E, 1); }
static void BIT1h(void) { bit(REG_H, 1); }
static void BIT1l(void) { bit(REG_L, 1); }
static void BIT1a(void) { bit(REG_A, 1); }
static void BIT1mHL(void) { bit(MMU_read_8(REG_HL), 1); }
static void BIT2b(void) { bit(REG_B, 2); }
static void BIT2c(void) { bit(REG_C, 2); }
static void BIT2d(void) { bit(REG_D, 2); }
static void BIT2e(void) { bit(REG_E, 2); }
static void BIT2h(void) { bit(REG_H, 2); }
static void BIT2l(void) { bit(REG_L, 2); }
static void BIT2a(void) { bit(REG_A, 2); }
static void BIT2mHL(void) { bit(MMU_read_8(REG_HL), 2); }
static void BIT3b(void) { bit(REG_B, 3); }
static void BIT3c(void) { bit(REG_C, 3); }
static void BIT3d(void) { bit(REG_D, 3); }
static void BIT3e(void) { bit(REG_E, 3); }
static void BIT3h(void) { bit(REG_H, 3); }
static void BIT3l(void) { bit(REG_L, 3); }
static void BIT3a(void) { bit(REG_A, 3); }
static void BIT3mHL(void) { bit(MMU_read_8(REG_HL), 3); }
static void BIT4b(void) { bit(REG_B, 4); }
static void BIT4c(void) { bit(REG_C, 4); }
static void BIT4d(void) { bit(REG_D, 4); }
static void BIT4e(void) { bit(REG_E, 4); }
static void BIT4h(void) { bit(REG_H, 4); }
static void BIT4l(void) { bit(REG_L, 4); }
static void BIT4a(void) { bit(REG_A, 4); }
static void BIT4mHL(void) { bit(MMU_read_8(REG_HL), 4); }
static void BIT5b(void) { bit(REG_B, 5); }
static void BIT5c(void) { bit(REG_C, 5); }
static void BIT5d(void) { bit(REG_D, 5); }
static void BIT5e(void) { bit(REG_E, 5); }
static void BIT5h(void) { bit(REG_H, 5); }
static void BIT5l(void) { bit(REG_L, 5); }
static void BIT5a(void) { bit(REG_A, 5); }
static void BIT5mHL(void) { bit(MMU_read_8(REG_HL), 5); }
static void BIT6b(void) { bit(REG_B, 6); }
static void BIT6c(void) { bit(REG_C, 6); }
static void BIT6d(void) { bit(REG_D, 6); }
static void BIT6e(void) { bit(REG_E, 6); }
static void BIT6h(void) { bit(REG_H, 6); }
static void BIT6l(void) { bit(REG_L, 6); }
static void BIT6a(void) { bit(REG_A, 6); }
static void BIT6mHL(void) { bit(MMU_read_8(REG_HL), 6); }
static void BIT7b(void) { bit(REG_B, 7); }
static void BIT7c(void) { bit(REG_C, 7); }
static void BIT7d(void) { bit(REG_D, 7); }
static void BIT7e(void) { bit(REG_E, 7); }
static void BIT7h(void) { bit(REG_H, 7); }
static void BIT7l(void) { bit(REG_L, 7); }
static void BIT7a(void) { bit(REG_A, 7); }
static void BIT7mHL(void) { bit(MMU_read_8(REG_HL), 7); }
// SET b, s
static void SET0b(void) { set(REG_B, 0); }
static void SET0c(void) { set(REG_C, 0); }
static void SET0d(void) { set(REG_D, 0); }
static void SET0e(void) { set(REG_E, 0); }
static void SET0h(void) { set(REG_H, 0); }
static void SET0l(void) { set(REG_L, 0); }
static void SET0a(void) { set(REG_A, 0); }
static void SET0mHL(void) { set(MMU_read_8(REG_HL), 0); }
static void SET1b(void) { set(REG_B, 1); }
static void SET1c(void) { set(REG_C, 1); }
static void SET1d(void) { set(REG_D, 1); }
static void SET1e(void) { set(REG_E, 1); }
static void SET1h(void) { set(REG_H, 1); }
static void SET1l(void) { set(REG_L, 1); }
static void SET1a(void) { set(REG_A, 1); }
static void SET1mHL(void) { set(MMU_read_8(REG_HL), 1); }
static void SET2b(void) { set(REG_B, 2); }
static void SET2c(void) { set(REG_C, 2); }
static void SET2d(void) { set(REG_D, 2); }
static void SET2e(void) { set(REG_E, 2); }
static void SET2h(void) { set(REG_H, 2); }
static void SET2l(void) { set(REG_L, 2); }
static void SET2a(void) { set(REG_A, 2); }
static void SET2mHL(void) { set(MMU_read_8(REG_HL), 2); }
static void SET3b(void) { set(REG_B, 3); }
static void SET3c(void) { set(REG_C, 3); }
static void SET3d(void) { set(REG_D, 3); }
static void SET3e(void) { set(REG_E, 3); }
static void SET3h(void) { set(REG_H, 3); }
static void SET3l(void) { set(REG_L, 3); }
static void SET3a(void) { set(REG_A, 3); }
static void SET3mHL(void) { set(MMU_read_8(REG_HL), 3); }
static void SET4b(void) { set(REG_B, 4); }
static void SET4c(void) { set(REG_C, 4); }
static void SET4d(void) { set(REG_D, 4); }
static void SET4e(void) { set(REG_E, 4); }
static void SET4h(void) { set(REG_H, 4); }
static void SET4l(void) { set(REG_L, 4); }
static void SET4a(void) { set(REG_A, 4); }
static void SET4mHL(void) { set(MMU_read_8(REG_HL), 4); }
static void SET5b(void) { set(REG_B, 5); }
static void SET5c(void) { set(REG_C, 5); }
static void SET5d(void) { set(REG_D, 5); }
static void SET5e(void) { set(REG_E, 5); }
static void SET5h(void) { set(REG_H, 5); }
static void SET5l(void) { set(REG_L, 5); }
static void SET5a(void) { set(REG_A, 5); }
static void SET5mHL(void) { set(MMU_read_8(REG_HL), 5); }
static void SET6b(void) { set(REG_B, 6); }
static void SET6c(void) { set(REG_C, 6); }
static void SET6d(void) { set(REG_D, 6); }
static void SET6e(void) { set(REG_E, 6); }
static void SET6h(void) { set(REG_H, 6); }
static void SET6l(void) { set(REG_L, 6); }
static void SET6a(void) { set(REG_A, 6); }
static void SET6mHL(void) { set(MMU_read_8(REG_HL), 6); }
static void SET7b(void) { set(REG_B, 7); }
static void SET7c(void) { set(REG_C, 7); }
static void SET7d(void) { set(REG_D, 7); }
static void SET7e(void) { set(REG_E, 7); }
static void SET7h(void) { set(REG_H, 7); }
static void SET7l(void) { set(REG_L, 7); }
static void SET7a(void) { set(REG_A, 7); }
static void SET7mHL(void) { set(MMU_read_8(REG_HL), 7); }
// RES b, s
static void RES0b(void) { res(REG_B, 0); }
static void RES0c(void) { res(REG_C, 0); }
static void RES0d(void) { res(REG_D, 0); }
static void RES0e(void) { res(REG_E, 0); }
static void RES0h(void) { res(REG_H, 0); }
static void RES0l(void) { res(REG_L, 0); }
static void RES0a(void) { res(REG_A, 0); }
static void RES0mHL(void) { res(MMU_read_8(REG_HL), 0); }
static void RES1b(void) { res(REG_B, 1); }
static void RES1c(void) { res(REG_C, 1); }
static void RES1d(void) { res(REG_D, 1); }
static void RES1e(void) { res(REG_E, 1); }
static void RES1h(void) { res(REG_H, 1); }
static void RES1l(void) { res(REG_L, 1); }
static void RES1a(void) { res(REG_A, 1); }
static void RES1mHL(void) { res(MMU_read_8(REG_HL), 1); }
static void RES2b(void) { res(REG_B, 2); }
static void RES2c(void) { res(REG_C, 2); }
static void RES2d(void) { res(REG_D, 2); }
static void RES2e(void) { res(REG_E, 2); }
static void RES2h(void) { res(REG_H, 2); }
static void RES2l(void) { res(REG_L, 2); }
static void RES2a(void) { res(REG_A, 2); }
static void RES2mHL(void) { res(MMU_read_8(REG_HL), 2); }
static void RES3b(void) { res(REG_B, 3); }
static void RES3c(void) { res(REG_C, 3); }
static void RES3d(void) { res(REG_D, 3); }
static void RES3e(void) { res(REG_E, 3); }
static void RES3h(void) { res(REG_H, 3); }
static void RES3l(void) { res(REG_L, 3); }
static void RES3a(void) { res(REG_A, 3); }
static void RES3mHL(void) { res(MMU_read_8(REG_HL), 3); }
static void RES4b(void) { res(REG_B, 4); }
static void RES4c(void) { res(REG_C, 4); }
static void RES4d(void) { res(REG_D, 4); }
static void RES4e(void) { res(REG_E, 4); }
static void RES4h(void) { res(REG_H, 4); }
static void RES4l(void) { res(REG_L, 4); }
static void RES4a(void) { res(REG_A, 4); }
static void RES4mHL(void) { res(MMU_read_8(REG_HL), 4); }
static void RES5b(void) { res(REG_B, 5); }
static void RES5c(void) { res(REG_C, 5); }
static void RES5d(void) { res(REG_D, 5); }
static void RES5e(void) { res(REG_E, 5); }
static void RES5h(void) { res(REG_H, 5); }
static void RES5l(void) { res(REG_L, 5); }
static void RES5a(void) { res(REG_A, 5); }
static void RES5mHL(void) { res(MMU_read_8(REG_HL), 5); }
static void RES6b(void) { res(REG_B, 6); }
static void RES6c(void) { res(REG_C, 6); }
static void RES6d(void) { res(REG_D, 6); }
static void RES6e(void) { res(REG_E, 6); }
static void RES6h(void) { res(REG_H, 6); }
static void RES6l(void) { res(REG_L, 6); }
static void RES6a(void) { res(REG_A, 6); }
static void RES6mHL(void) { res(MMU_read_8(REG_HL), 6); }
static void RES7b(void) { res(REG_B, 7); }
static void RES7c(void) { res(REG_C, 7); }
static void RES7d(void) { res(REG_D, 7); }
static void RES7e(void) { res(REG_E, 7); }
static void RES7h(void) { res(REG_H, 7); }
static void RES7l(void) { res(REG_L, 7); }
static void RES7a(void) { res(REG_A, 7); }
static void RES7mHL(void) { res(MMU_read_8(REG_HL), 7); }

// Jumps
// JP nn
static void JPnn(void) { cpu.operand = MMU_read_16(REG_PC); REG_PC = cpu.operand; }
// JP cc, nn
static void JPZnn(void) {
	if (FLAG_Z) {
		cpu.operand = MMU_read_16(REG_PC);
		REG_PC = cpu.operand;
		cpu.ins_clock.m = 4;
		return;
	}

	REG_PC += 2;
}
static void JPCnn(void) {
	if (FLAG_C) {
		cpu.operand = MMU_read_16(REG_PC);
		REG_PC = cpu.operand;
		cpu.ins_clock.m = 4;
		return;
	}

	REG_PC += 2;
}
static void JPNZnn(void) {
	if (!FLAG_Z) {
		cpu.operand = MMU_read_16(REG_PC);
		REG_PC = cpu.operand;
		cpu.ins_clock.m = 4;
		return;
	}

	REG_PC += 2;
}
static void JPNCnn(void) {
	if (!FLAG_C) {
		cpu.operand = MMU_read_16(REG_PC);
		REG_PC = cpu.operand;
		cpu.ins_clock.m = 4;
		return;
	}

	REG_PC += 2;
}
// JP (HL)
static void JPmHL(void) {
	// Why is this called JP (HL) and not JP HL?
	REG_PC = REG_HL;
}
// JR e
static void JRn(void) { cpu.operand = MMU_read_8(REG_PC); jr(cpu.operand); REG_PC++; }
// JR cc, e
static void JRZn(void) {
	if (FLAG_Z) {
		cpu.operand = MMU_read_8(REG_PC);
		jr(cpu.operand);
		REG_PC++;
		cpu.ins_clock.m = 4;
		return;
	}

	REG_PC++;
}
static void JRCn(void) {
	if (FLAG_C) {
		cpu.operand = MMU_read_8(REG_PC);
		jr(cpu.operand);
		REG_PC++;
		cpu.ins_clock.m = 4;
		return;
	}

	REG_PC++;
}
static void JRNZn(void) {
	if (!FLAG_Z) {
		cpu.operand = MMU_read_8(REG_PC);
		jr(cpu.operand);
		REG_PC++;
		cpu.ins_clock.m = 4;
		return;
	}

	REG_PC++;
}
static void JRNCn(void) {
	if (!FLAG_C) {
		cpu.operand = MMU_read_8(REG_PC);
		jr(cpu.operand);
		REG_PC++;
		cpu.ins_clock.m = 4;
		return;
	}

	REG_PC++;
}

// Calls
// CALL nn
static void CALLnn(void) { call(MMU_read_16(REG_PC)); }
// CALL cc, nn
static void CALLZnn(void) {
	if (FLAG_Z) {
		cpu.operand = MMU_read_8(REG_PC);
		call(cpu.operand);
		cpu.ins_clock.m = 12;
		return;
	}

	REG_PC += 2;
}
static void CALLCnn(void) {
	if (FLAG_C) {
		cpu.operand = MMU_read_8(REG_PC);
		call(cpu.operand);
		cpu.ins_clock.m = 12;
		return;
	}

	REG_PC += 2;
}
static void CALLNZnn(void) {
	if (!FLAG_Z) {
		cpu.operand = MMU_read_8(REG_PC);
		call(cpu.operand);
		cpu.ins_clock.m = 12;
		return;
	}

	REG_PC += 2;
}
static void CALLNCnn(void) {
	if (!FLAG_C) {
		cpu.operand = MMU_read_8(REG_PC);
		call(cpu.operand);
		cpu.ins_clock.m = 12;
		return;
	}

	REG_PC += 2;
}

// Restarts
// RST f
static void RST0(void) { rst(0x00); }
static void RST8(void) { rst(0x08); }
static void RST10(void) { rst(0x10); }
static void RST18(void) { rst(0x18); }
static void RST20(void) { rst(0x20); }
static void RST28(void) { rst(0x28); }
static void RST30(void) { rst(0x30); }
static void RST38(void) { rst(0x38); }

// Returns
// RET
static void RET(void) { ret(); }
// RET cc
static void RETZ(void) {
	if (FLAG_Z) {
		ret();
		cpu.ins_clock.m = 12;
	}
}
static void RETC(void) {
	if (FLAG_C) {
		ret();
		cpu.ins_clock.m = 12;
	}
}
static void RETNZ(void) {
	if (!FLAG_Z) {
		ret();
		cpu.ins_clock.m = 12;
	}
}
static void RETNC(void)
{
	if (!FLAG_C) {
		ret();
		cpu.ins_clock.m = 12;
	}
}
// RETI
static void RETI(void) { ret(); cpu.ime = 1; }

// Lookup table for two-byte opcodes
static void (*cb_ops[256])() = {
/*   |   0   |   1  |   2  |   3  |   4  |   5  |	6   |   7  |   8  |   9  |   A  |   B  |   C  |   D  |	E   |   F  | */
/* 0 */ RLCb , RLCc , RLCd , RLCe , RLCh , RLCl , RLCmHL , RLCa , RRCb , RRCc , RRCd , RRCe , RRCh , RRCl , RRCmHL , RRCa ,
/* 1 */ RLb  , RLc  , RLd  , RLe  , RLh  , RLl  , RLmHL  , RLa  , RRb  , RRc  , RRd  , RRe  , RRh  , RRl  , RRmHL  , RRa  ,
/* 2 */ SLAb , SLAc , SLAd , SLAe , SLAh , SLAl , SLAmHL , SLAa , SRAb , SRAc , SRAd , SRAe , SRAh , SRAl , SRAmHL , SRAa ,
/* 3 */ SWAPb, SWAPc, SWAPd, SWAPe, SWAPh, SWAPl, SWAPmHL, SWAPa, SRLb , SRLc , SRLd , SRLe , SRLh , SRLl , SRLmHL , SRLa ,
/* 4 */ BIT0b, BIT0c, BIT0d, BIT0e, BIT0h, BIT0l, BIT0mHL, BIT0a, BIT1b, BIT1c, BIT1d, BIT1e, BIT1h, BIT1l, BIT1mHL, BIT1a,
/* 5 */ BIT2b, BIT2c, BIT2d, BIT2e, BIT2h, BIT2l, BIT2mHL, BIT2a, BIT3b, BIT3c, BIT3d, BIT3e, BIT3h, BIT3l, BIT3mHL, BIT3a,
/* 6 */ BIT4b, BIT4c, BIT4d, BIT4e, BIT4h, BIT4l, BIT4mHL, BIT4a, BIT5b, BIT5c, BIT5d, BIT5e, BIT5h, BIT5l, BIT5mHL, BIT5a,
/* 7 */ BIT6b, BIT6c, BIT6d, BIT6e, BIT6h, BIT6l, BIT6mHL, BIT6a, BIT7b, BIT7c, BIT7d, BIT7e, BIT7h, BIT7l, BIT7mHL, BIT7a,
/* 8 */ RES0b, RES0c, RES0d, RES0e, RES0h, RES0l, RES0mHL, RES0a, RES1b, RES1c, RES1d, RES1e, RES1h, RES1l, RES1mHL, RES1a,
/* 9 */ RES2b, RES2c, RES2d, RES2e, RES2h, RES2l, RES2mHL, RES2a, RES3b, RES3c, RES3d, RES3e, RES3h, RES3l, RES3mHL, RES3a,
/* A */ RES4b, RES4c, RES4d, RES4e, RES4h, RES4l, RES4mHL, RES4a, RES5b, RES5c, RES5d, RES5e, RES5h, RES5l, RES5mHL, RES5a,
/* B */ RES6b, RES6c, RES6d, RES6e, RES6h, RES6l, RES6mHL, RES6a, RES7b, RES7c, RES7d, RES7e, RES7h, RES7l, RES7mHL, RES7a,
/* 8 */ SET0b, SET0c, SET0d, SET0e, SET0h, SET0l, SET0mHL, SET0a, SET1b, SET1c, SET1d, SET1e, SET1h, SET1l, SET1mHL, SET1a,
/* 9 */ SET2b, SET2c, SET2d, SET2e, SET2h, SET2l, SET2mHL, SET2a, SET3b, SET3c, SET3d, SET3e, SET3h, SET3l, SET3mHL, SET3a,
/* A */ SET4b, SET4c, SET4d, SET4e, SET4h, SET4l, SET4mHL, SET4a, SET5b, SET5c, SET5d, SET5e, SET5h, SET5l, SET5mHL, SET5a,
/* B */ SET6b, SET6c, SET6d, SET6e, SET6h, SET6l, SET6mHL, SET6a, SET7b, SET7c, SET7d, SET7e, SET7h, SET7l, SET7mHL, SET7a
};

// Jump to 2-byte opcodes
static void CB(void) {
	cpu.cb_op = MMU_read_8(REG_PC++);
	(*cb_ops[cpu.cb_op])();
}

// Table of function pointers indexed by opcode
static void (*ops[256])() = {
/*    |    0   |   1   |    2   |   3   |    4    |   5   |    6   |   7   |    8   |    9   |    A   |   B  |    C   |   D   |    E   |   F  | */
/* 0 */ NOP    , LDBCnn, LDmBCa , INCBC , INCb    , DECb  , LDbn   , RLCA  , LDmnnSP, ADDHLBC, LDamBC , DECBC, INCc   , DECc  , LDcn   , RRCA ,
/* 1 */ STOP   , LDDEnn, LDmDEa , INCDE , INCd    , DECd  , LDdn   , RLA   , JRn    , ADDHLDE, LDamDE , DECDE, INCe   , DECe  , LDen   , RRA  ,
/* 2 */ JRNZn  , LDHLnn, LDImHLa, INCHL , INCh    , DECh  , LDhn   , DAA   , JRZn   , ADDHLHL, LDIamHL, DECHL, INCl   , DECl  , LDln   , CPL  ,
/* 3 */ JRNCn  , LDSPnn, LDDmHLa, INCSP , INCmHL  , DECmHL, LDmHLn , SCF   , JRCn   , ADDHLSP, LDDamHL, DECSP, INCa   , DECa  , LDan   , CCF  ,
/* 4 */ LDbb   , LDbc  , LDbd   , LDbe  , LDbh    , LDbl  , LDbmHL , LDba  , LDcb   , LDcc   , LDcd   , LDce , LDch   , LDcl  , LDcmHL , LDca ,
/* 5 */ LDdb   , LDdc  , LDdd   , LDde  , LDdh    , LDdl  , LDdmHL , LDda  , LDeb   , LDec   , LDed   , LDee , LDeh   , LDel  , LDemHL , LDea ,
/* 6 */ LDhb   , LDhc  , LDhd   , LDhe  , LDhh    , LDhl  , LDhmHL , LDha  , LDlb   , LDlc   , LDld   , LDle , LDlh   , LDll  , LDlmHL , LDla ,
/* 7 */ LDmHLb , LDmHLc, LDmHLd , LDmHLe, LDmHLh  , LDmHLl, HALT   , LDmHLa, LDab   , LDac   , LDad   , LDae , LDah   , LDal  , LDamHL , LDaa ,
/* 8 */ ADDab  , ADDac , ADDad  , ADDae , ADDah   , ADDal , ADDamHL, ADDaa , ADCab  , ADCac  , ADCad  , ADCae, ADCah  , ADCal , ADCamHL, ADCaa,
/* 9 */ SUBab  , SUBac , SUBad  , SUBae , SUBah   , SUBal , SUBamHL, SUBaa , SBCab  , SBCac  , SBCad  , SBCae, SBCah  , SBCal , SBCamHL, SBCaa,
/* A */ ANDb   , ANDc  , ANDd   , ANDe  , ANDh    , ANDl  , ANDmHL , ANDa  , XORb   , XORc   , XORd   , XORe , XORh   , XORl  , XORmHL , XORa ,
/* B */ ORb    , ORc   , ORd    , ORe   , ORh     , ORl   , ORmHL  , ORa   , CPb    , CPc    , CPd    , CPe  , CPh    , CPl   , CPmHL  , CPa  ,
/* C */ RETNZ  , POPBC , JPNZnn , JPnn  , CALLNZnn, PUSHBC, ADDan  , RST0  , RETZ   , RET    , JPZnn  , CB   , CALLZnn, CALLnn, ADCan  , RST8 ,
/* D */ RETNC  , POPDE , JPNCnn , NOP   , CALLNCnn, PUSHDE, SUBan  , RST10 , RETC   , RETI   , JPCnn  , NOP  , CALLCnn, NOP   , SBCan  , RST18,
/* E */ LDHmna , POPHL , LDmca  , NOP   , NOP     , PUSHHL, ANDn   , RST20 , ADDSPn , JPmHL  , LDmnna , NOP  , NOP    , NOP   , XORn   , RST28,
/* F */ LDHamn , POPAF , LDamc  , DI    , NOP     , PUSHAF, ORn    , RST30 , LDHLSPn, LDSPHL , LDamnn , EI   , NOP    , NOP   , CPn    , RST38
};

// M clock values
BYTE timings_m[256] = {
/*   |  0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B| C | D | E| F | */
/* 0 */  4, 12,  8,  8,  4,  4,  8,  4, 20,  8,  8, 8,  4,  4, 8,  4,
/* 1 */  4, 12,  8,  8,  4,  4,  8,  4, 12,  8,  8, 8,  4,  4, 8,  4,
/* 2 */  8, 12,  8,  8,  4,  4,  8,  4,  8,  8,  8, 8,  4,  4, 8,  4,
/* 3 */  8, 12,  8,  8, 12, 12, 12,  4,  8,  8,  8, 8,  4,  4, 8,  4,
/* 4 */  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4, 4,  4,  4, 8,  4,
/* 5 */  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4, 4,  4,  4, 8,  4,
/* 6 */  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4, 4,  4,  4, 8,  4,
/* 7 */  8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4, 4,  4,  4, 8,  4,
/* 8 */  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4, 4,  4,  4, 8,  4,
/* 9 */  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4, 4,  4,  4, 8,  4,
/* A */  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4, 4,  4,  4, 8,  4,
/* B */  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4, 4,  4,  4, 8,  4,
/* C */  8, 12, 12, 16, 12, 16,  8, 16,  8, 16, 12, 4, 12, 24, 8, 16,
/* D */  8, 12, 12,  4, 12, 16,  8, 16,  8, 16, 12, 4, 12,  4, 8, 16,
/* E */ 12, 12,  8,  4,  4, 16,  8, 16, 16,  4, 16, 4,  4,  4, 8, 16,
/* F */ 12, 12,  8,  4,  4, 16,  8, 16, 12,  8, 16, 4,  4,  4, 8, 16
};

// M clock values (0xCB prefix)
BYTE cb_timings_m[256] = {
/*   | 0 | 1| 2| 3| 4| 5| 6 | 7| 8| 9| A| B| C| D| E | F| */
/* 0 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 1 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 2 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 3 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 4 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 5 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 6 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 7 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 8 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 9 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* A */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* B */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* C */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* D */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* E */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* F */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8
};


// T clock values
BYTE timings_t[/*256*/] = {
	0 // Stuff
};

// Executive functions
void CPU_reset(void) {
	FLAG_Z = 1;
	FLAG_N = 0;
	FLAG_H = 1;
	FLAG_C = 1;

	REG_BC = 0x0013;
	REG_DE = 0x00D8;
	REG_HL = 0x014D;

	REG_PC = 0x0100;
	REG_SP = 0xFFFE;

	REG_A = 0x01;

	cpu.op = 0;

	cpu.sys_clock.m = 0;
	cpu.sys_clock.t = 0;
	cpu.ins_clock.m = 0;
	cpu.ins_clock.t = 0;
}

int CPU_step() {
	cpu.ins_clock.m = 0;

	// TODO: Interrupts

	cpu.op = MMU_read_8(REG_PC++);

	(*ops[cpu.op])();

	if (cpu.op != 0xCB) {
		cpu.ins_clock.m += timings_m[cpu.op];
		cpu.ins_clock.t = timings_t[cpu.op];
	} else {
		cpu.ins_clock.m += cb_timings_m[cpu.cb_op];
	}
	/* TODO: ^ replace with:
		cpu.ins_clock.t = CEIL(cpu.ins_clock.m / 4.0)
	*/
	cpu.sys_clock.m += cpu.ins_clock.m;
	cpu.sys_clock.t += cpu.ins_clock.t;

	// Stash the CPU state
	cpu_history.ptr = cpu_history.ptr == 0xFFFF ? 0 : cpu_history.ptr + 1;
	memcpy(&(cpu_history.states[cpu_history.ptr]), &cpu, sizeof(cpu));

	// Update the debugger
	Debugger_update();

	return cpu.ins_clock.m;
}

int CPU_run(DWORD cycles) {
	DWORD total = 0;

	while (total < cycles) {
		total += CPU_step();
	}

	return 0;
}
