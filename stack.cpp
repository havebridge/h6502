#include "6502.h"


word SPtoWord(struct CPU* cpu)
{
	return 0x100 | cpu->sp;
}

void pushPCToStack(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	WriteWord(SPtoWord(cpu) - 1, cpu->pc - 1, mem, Cycles);
	cpu->sp -= 2;
}

word popWordFromStack(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	word ReturnValue = ReadWord(mem, SPtoWord(cpu) + 1, Cycles);
	cpu->sp += 2;
	(*Cycles)--;
	return ReturnValue;
}

void pushByteOntoStack(byte value, struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	mem->Data[SPtoWord(cpu)] = value;
	cpu->sp--;
	(*Cycles) -= 2;
}

byte popByteOntoStack(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	byte value = mem->Data[SPtoWord(cpu)];
	cpu->sp++;
	(*Cycles)--;
	return value;
}