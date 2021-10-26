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