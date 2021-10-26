#ifndef M6502_H
#define M6502_H
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#define byte unsigned char
#define word unsigned short

#define MAX_MEM 1024 * 64	// the amount of ram

enum INSTRS
{
	LDA_IM = 0xA9,			// load accumulator (immidiately addressing mode)
	LDA_ZP = 0xA5,			// load accumulator (zero page addressing mode)
	LDA_ZPX = 0xB5,			// load accumulator (zero page + x register addressing mode)
	LDA_ABS = 0xAD,			// load accumulator (absolute addressing mode)
	LDA_ABSX = 0xBD,		// load accumulator (absolute addressing mode + x register)
	LDA_ABSY = 0xB9,		// load accumulator (absolute addressing mode + y register)
	LDA_INDX = 0xA1,		// load accumulator (indexed-indirect addressing mode with x register)
	LDA_INDY = 0xB1,		// load accumulator (indexed-indirect addressing mode with y register)

	LDX_IM = 0xA2,			// load x register	(immidiately addressing mode)
	LDX_ZP = 0xA6,			// load x register  (zero page addressing mode)
	LDX_ZPY = 0xB6,			// load x register  (zero page + y register addressing mode)
	LDX_ABS = 0xAE,			// load x register	(absolute addressing mode)
	LDX_ABSY = 0xBE,		// load x register  (absolute addressing mode + y register)

	LDY_IM = 0xA0,			// load y register  (immidiately addressing mode)
	LDY_ZP = 0xA4,			// load y register	(zero page addressing mode)
	LDY_ZPX = 0xB4,			// load y register  (zero page + x register addressing mode)
	LDY_ABS = 0xAC,			// load y register	(absolute addressing mode)
	LDY_ABSX = 0xBC,		// load y register  (absolute addressing mode + x register)

	STA_ZP = 0x85,			// store accumulator into memory (zero page addressing mode)
	STA_ZPX = 0x95,			// store accumulator into memory (zero page + x register addressing mode)
	STA_ABS = 0x8D,			// store accumulator into memory (absolute addressing mode)
	STA_ABSX = 0x9D,		// store accumulator into memory (absolute addressing mode + x register)
	STA_ABSY = 0x99,		// store accumulator into memory (absolute addressing mode + y register)
	STA_INDX = 0x81,		// store accumulator (indexed-indirect addressing mode with x register)
	STA_INDY = 0x91,		// store accumulator (indexed-indirect addressing mode with y register)

	STX_ZP = 0x86,			// store x register into memory	 (zero page addressing mode)
	STX_ZPY = 0x96,			// store x register into memory  (zero page + y register addressing mode)
	STX_ABS = 0x8E,			// store x register into memory	 (absolute addressing mode)

	STY_ZP = 0x87,			// store y register into memory  (zero page addressing mode)
	STY_ZPX = 0x94,			// store y register into memory  (zero page + x register addressing mode)
	STY_ABS = 0x8C,			// store y register into memory  (absolute addressing mode)

	JSR = 0x20,			// jump to subroutine
	RTS = 0x60			// return from subroutine
};

enum FLAGS
{
	carryFlag = 0,
	zeroFlag,
	interruptDisable,
	decimalMode,
	breakCommand,
	overflowFlag,
	negativeFlag
};

struct CPU
{
	word pc;				// program counter
	byte sp;				// stack pointer

	byte acc;				// accumulator
	byte x;
	byte y;

	byte Flags[negativeFlag];
};


struct memory
{
	byte Data[MAX_MEM];
};

void WriteWord(const word, const word, struct memory*, size_t*);
word ReadWord(struct memory*, const word, size_t*);

word SPtoWord(struct CPU*);
void pushPCToStack(struct CPU*, struct memory*, size_t*);
word popWordFromStack(struct CPU*, struct memory*, size_t*);


void ResetCpu(struct CPU* cpu, struct memory* mem);
uint32_t Execute(struct CPU* cpu, struct memory* mem, size_t cycles);

#endif