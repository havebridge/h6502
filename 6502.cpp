#include "6502.h"


static byte FetchByte(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	byte Data = mem->Data[cpu->pc++];
	(*Cycles)--;
	return Data;
}


static byte ReadByte(const word Address, struct memory* mem, size_t* Cycles)
{
	byte Data = mem->Data[Address];
	(*Cycles)--;
	return Data;
}


static void WriteByte(const word Address, const word data, struct memory* mem, size_t* Cycles)
{
	mem->Data[Address] = data;
	(*Cycles)--;
}

static word FetchWord(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	word Data = mem->Data[cpu->pc];
	cpu->pc++;

	Data |= (mem->Data[cpu->pc] << 8);
	cpu->pc++;
	(*Cycles) -= 2;
	return Data;
}

void WriteWord(const word Address, const word data, struct memory* mem, size_t* Cycles)
{
	mem->Data[Address] = data & 0x00FF;
	mem->Data[Address + 1] = data >> 8;
	(*Cycles) -= 2;
}

word ReadWord(struct memory* mem, const word Address, size_t* cycles)
{
	byte LowByte = ReadByte(Address, mem, cycles);
	byte HighByte = ReadByte(Address + 1, mem, cycles);
	return LowByte | (HighByte << 8);
}

static void SetStatusFlags(struct CPU* cpu, const byte reg)
{
	cpu->Flags[zeroFlag] = (reg == 0);
	cpu->Flags[negativeFlag - 1] = (reg & 0x80) < 0; // not working because number that are bigger than 128 will detect as negative
}


void ResetCpu(struct CPU* cpu, struct memory* mem)
{
	cpu->pc = 0xFFFC;
	cpu->sp = 0xFF;
	cpu->acc = cpu->x = cpu->y = 0;

	memset(cpu->Flags, 0, sizeof(cpu->Flags));
	memset(mem->Data, 0, sizeof(mem->Data));
	
	memset(mem->Data, 0, sizeof(mem->Data));
}

uint32_t Execute(struct CPU* cpu, struct memory* mem, uint32_t cycles)
{
	const size_t numCycles = cycles;
	while (cycles > 0)
	{
		byte Instruction = FetchByte(cpu, mem, &cycles);

		switch (Instruction)
		{
		case LDA_IM:
		{
			cpu->acc = FetchByte(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_ZP:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			cpu->acc = ReadByte(ZeroPageAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_ZPX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			cycles--;
			cpu->acc = ReadByte(ZeroPageAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_ABS:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			cpu->acc = ReadByte(AbsAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_ABSX:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressX = AbsAddress + cpu->x;
			cpu->acc = ReadByte(AbsAddressX, mem, &cycles);
			if (AbsAddressX - AbsAddress >= 0xFF)
			{
				cycles--;
			}
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_ABSY:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressY = AbsAddress + cpu->y;
			cpu->acc = ReadByte(AbsAddressY, mem, &cycles);
			if (AbsAddressY - AbsAddress >= 0xFF)
			{
				cycles--;
			}
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_INDX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			cycles--;
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			cpu->acc = ReadByte(EffectiveAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_INDY:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			cpu->acc = ReadByte(EffectiveAddress + cpu->y, mem, &cycles);
			if ((EffectiveAddress + cpu->y) - EffectiveAddress > 0xFF)
			{
				cycles--;
			}
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDX_IM:
		{
			cpu->x = FetchByte(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case LDX_ZP:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			cpu->x = ReadByte(ZeroPageAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case LDX_ZPY:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->y;
			cycles--;
			cpu->x = ReadByte(ZeroPageAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case LDX_ABS:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			cpu->x = ReadByte(AbsAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case LDX_ABSY:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressY = AbsAddress + cpu->y;
			cpu->x = ReadByte(AbsAddressY, mem, &cycles);
			if (AbsAddressY - AbsAddress >= 0xFF)
			{
				cycles--;
			}
			SetStatusFlags(cpu, cpu->x);
		} break;
		case LDY_IM:
		{
			cpu->y = FetchByte(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case LDY_ZP:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			cpu->y = ReadByte(ZeroPageAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case LDY_ZPX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			cycles--;
			cpu->y = ReadByte(ZeroPageAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case LDY_ABS:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			cpu->y = ReadByte(AbsAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case LDY_ABSX:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressX = AbsAddress + cpu->x;
			cpu->y = ReadByte(AbsAddressX, mem, &cycles);
			if (AbsAddressX - AbsAddress >= 0xFF)
			{
				cycles--;
			}
			SetStatusFlags(cpu, cpu->y);
		} break;
		case STA_ZP:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			WriteByte(ZeroPageAddress, cpu->acc, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case STA_ZPX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			WriteByte(ZeroPageAddress, cpu->acc, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case STX_ZP:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			WriteByte(ZeroPageAddress, cpu->x, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case STX_ZPY:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->y;
			WriteByte(ZeroPageAddress, cpu->x, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case STY_ZP:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			WriteByte(ZeroPageAddress, cpu->y, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case STY_ZPX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			WriteByte(ZeroPageAddress, cpu->y, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case STA_ABS:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			WriteWord(AbsAddress, cpu->acc, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case STX_ABS:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			WriteWord(AbsAddress, cpu->x, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case STY_ABS:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			WriteWord(AbsAddress, cpu->y, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case STA_ABSX:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressX = AbsAddress + cpu->x;
			WriteWord(AbsAddressX, cpu->acc, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case STA_ABSY:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressY = AbsAddress + cpu->y;
			WriteWord(AbsAddressY, cpu->acc, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case STA_INDX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			cycles--;
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			WriteByte(EffectiveAddress, cpu->acc, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case STA_INDY:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			WriteByte(EffectiveAddress + cpu->y, cpu->acc, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case JSR:
		{
			word SubroutineAddress = FetchWord(cpu, mem, &cycles);
			pushPCToStack(cpu, mem, &cycles);
			cpu->pc = SubroutineAddress;
			cycles--;
		} break;
		case RTS:
		{
			word ReturnAddress = popWordFromStack(cpu, mem, &cycles);
			cpu->pc = ReturnAddress + 1;
			cycles -= 2;
		} break;
		default:
		{
			printf("Instruction not handled %d\n", Instruction);
		} break;
		}
	}

	return numCycles - cycles;
}