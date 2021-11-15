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

	TSX = 0xBA,			// transfer stack pointer to x register
	TXS = 0x9A,			// transfer x register to stack pointer
	PHA = 0x48,			// push accumulator onto the stack
	PHP = 0x08,			// push copy of the status flags on to the stack
	PLA = 0x68,			// pull accumulator from the stack
	PLP = 0x28,			// pull copy of the status flags from the stack

	AND_IM = 0x29,			// logical AND operation (immidiately addressing mode)
	AND_ZP = 0x25,			// logical AND operation (zero page addressing mode)
	AND_ZPX = 0x35,			// logical AND operation (zero page + x register addressing mode)
	AND_ABS = 0x2D,			// logical AND operation (absolute addressing mode)
	AND_ABSX = 0x3D,		// logical AND operation (absolute addressing mode + x register)
	AND_ABSY = 0x39,		// logical AND operation (absolute addressing mode + y register)
	AND_INDX = 0x21,		// logical AND operation (indexed-indirect addressing mode with x register)
	AND_INDY = 0x31,		// logical AND operation (indexed-indirect addressing mode with y register)

	OR_IM = 0x09,			// logical OR operation (immidiately addressing mode)
	OR_ZP = 0x05,			// logical OR operation (zero page addressing mode)
	OR_ZPX = 0x15,			// logical OR operation (zero page + x register addressing mode)
	OR_ABS = 0x0D,			// logical OR operation (absolute addressing mode)
	OR_ABSX = 0x1D,			// logical OR operation (absolute addressing mode + x register)
	OR_ABSY = 0x19,			// logical OR operation (absolute addressing mode + y register)
	OR_INDX = 0x01,			// logical OR operation (indexed-indirect addressing mode with x register)
	OR_INDY = 0x11,			// logical OR operation (indexed-indirect addressing mode with y register)

	EOR_IM = 0x49,			// logical EOR operation (immidiately addressing mode) 
	EOR_ZP = 0x45,			// logical EOR operation (zero page addressing mode)
	EOR_ZPX = 0x55,			// logical EOR operation (zero page + x register addressing mode)
	EOR_ABS = 0x4D,			// logical EOR operation (absolute addressing mode)
	EOR_ABSX = 0x5D,		// logical EOR operation (absolute addressing mode + x register)
	EOR_ABSY = 0x59,		// logical EOR operation (absolute addressing mode + y register)
	EOR_INDX = 0x41,		// logical EOR operation (indexed-indirect addressing mode with x register)
	EOR_INDY = 0x51,		// logical EOR operation (indexed-indirect addressing mode with y register)

	TAX_IM = 0xAA,			// Transfer accumulator to x register 
	TAY_IM = 0xA8,			// Transfer accumulator to y register 
	TXA_IM = 0x8A,			// Transfer x register to accumulator 
	TYA_IM = 0x98,			// Transfer y register to accumulator 

	INX_IM = 0xE8,			// Increment x register
	DEX_IM = 0xCA,			// Decrement x register
	INY_IM = 0xC8,			// Increment y register
	DEY_IM = 0x88,			// Decrement y register

	JSR = 0x20,			// jump to subroutine
	RTS = 0x60,			// return from subroutine
	JMP_ABS = 0x4C,			// jump to address (absolute addressing mode)
	JMP_IND = 0x6C			// jump to address (indexed-indirect addressing mode)
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
void pushByteOntoStack(byte, struct CPU*, struct memory*, size_t*);
byte popByteOntoStack(struct CPU*, struct memory*, size_t*);

void ResetCpu(struct CPU* cpu, struct memory* mem);
uint32_t Execute(struct CPU* cpu, struct memory* mem, size_t cycles);

#endif