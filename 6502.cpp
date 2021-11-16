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
	cpu->Flags[negativeFlag - 1] = (reg & 0x80) > 0; // not working because number that are bigger than 128 will detect as negative
}

static byte ZeroPage(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	byte ZeroPageAddress = FetchByte(cpu, mem, Cycles);
	return ReadByte(ZeroPageAddress, mem, Cycles);
}

static byte ZeroPageX(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	byte ZeroPageAddress = FetchByte(cpu, mem, Cycles);
	ZeroPageAddress += cpu->x;
	Cycles--;
	return ReadByte(ZeroPageAddress, mem, Cycles);
}

static byte ZeroPageY(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	byte ZeroPageAddress = FetchByte(cpu, mem, Cycles);
	ZeroPageAddress += cpu->y;
	Cycles--;
	return ReadByte(ZeroPageAddress, mem, Cycles);
}


static byte Absolute(struct CPU* cpu, struct memory* mem, size_t* Cycles)
{
	word AbsAddress = FetchWord(cpu, mem, Cycles);
	return ReadByte(AbsAddress, mem, Cycles);
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
			cpu->acc = ZeroPage(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_ZPX:
		{
			cpu->acc = ZeroPageX(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case LDA_ABS:
		{
			cpu->acc = Absolute(cpu, mem, &cycles);
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
			cpu->x = ZeroPage(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case LDX_ZPY:
		{
			cpu->x = ZeroPageY(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->x);
		} break;
		case LDX_ABS:
		{
			cpu->x = Absolute(cpu, mem, &cycles);
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
			cpu->y = ZeroPage(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case LDY_ZPX:
		{
			cpu->y = ZeroPageX(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->y);
		} break;
		case LDY_ABS:
		{
			cpu->y = Absolute(cpu, mem, &cycles);
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
		case TSX:
		{
			cpu->x = cpu->sp;
			cycles--;
			SetStatusFlags(cpu, cpu->x);
		} break;
		case TXS:
		{
			cpu->sp = cpu->x;
			cycles--;
		} break;
		case PHA:
		{
			pushByteOntoStack(cpu->acc, cpu, mem, &cycles);
		} break;
		case PHP:
		{
			pushByteOntoStack(*cpu->Flags, cpu, mem, &cycles);
		} break;
		case PLA:
		{
			cpu->acc = popByteOntoStack(cpu, mem, &cycles);
		} break;
		case PLP:
		{
			*cpu->Flags = popByteOntoStack(cpu, mem, &cycles);
		} break;
		case AND_IM:
		{
			cpu->acc &= FetchByte(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case OR_IM:
		{
			cpu->acc |= FetchByte(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case EOR_IM:
		{
			cpu->acc ^= FetchByte(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case AND_ZP:
		{
			cpu->acc &= ZeroPage(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case OR_ZP:
		{
			cpu->acc |= ZeroPage(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case EOR_ZP:
		{
			cpu->acc ^= ZeroPage(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case AND_ZPX:
		{
			cpu->acc &= ZeroPageX(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case OR_ZPX:
		{
			cpu->acc |= ZeroPageX(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case EOR_ZPX:
		{
			cpu->acc ^= ZeroPageX(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case AND_ABS:
		{
			cpu->acc &= Absolute(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case OR_ABS:
		{
			cpu->acc |= Absolute(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case EOR_ABS:
		{
			cpu->acc ^= Absolute(cpu, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case AND_ABSX:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressX = AbsAddress + cpu->x;
			cpu->acc &= ReadByte(AbsAddressX, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case OR_ABSX:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressX = AbsAddress + cpu->x;
			cpu->acc |= ReadByte(AbsAddressX, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case EOR_ABSX:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressX = AbsAddress + cpu->x;
			cpu->acc ^= ReadByte(AbsAddressX, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case AND_ABSY:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressY = AbsAddress + cpu->y;
			cpu->acc &= ReadByte(AbsAddressY, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case OR_ABSY:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressY = AbsAddress + cpu->y;
			cpu->acc |= ReadByte(AbsAddressY, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case EOR_ABSY:
		{
			word AbsAddress = FetchWord(cpu, mem, &cycles);
			word AbsAddressY = AbsAddress + cpu->y;
			cpu->acc ^= ReadByte(AbsAddressY, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case AND_INDX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			cycles--;
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			cpu->acc &= ReadByte(EffectiveAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case OR_INDX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			cycles--;
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			cpu->acc |= ReadByte(EffectiveAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case EOR_INDX:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			ZeroPageAddress += cpu->x;
			cycles--;
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			cpu->acc ^= ReadByte(EffectiveAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case AND_INDY:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			EffectiveAddress += cpu->y;
			cpu->acc &= ReadByte(EffectiveAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case OR_INDY:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			EffectiveAddress += cpu->y;
			cpu->acc |= ReadByte(EffectiveAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case EOR_INDY:
		{
			byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
			word EffectiveAddress = ReadWord(mem, ZeroPageAddress, &cycles);
			EffectiveAddress += cpu->y;
			cpu->acc ^= ReadByte(EffectiveAddress, mem, &cycles);
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case TAX_IM:
		{
			cpu->x = cpu->acc;
			cycles--;
			SetStatusFlags(cpu, cpu->x);
		} break;
		case TAY_IM:
		{
			cpu->y = cpu->acc;
			cycles--;
			SetStatusFlags(cpu, cpu->y);
		} break;
		case TXA_IM:
		{
			cpu->acc = cpu->x;
			cycles--;
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case TYA_IM:
		{
			cpu->acc = cpu->y;
			cycles--;
			SetStatusFlags(cpu, cpu->acc);
		} break;
		case INX_IM:
		{
			cpu->x++;
			cycles--;
			SetStatusFlags(cpu, cpu->x);
		} break;
		case INY_IM:
		{
			cpu->y++;
			cycles--;
			SetStatusFlags(cpu, cpu->y);
		} break;
		case DEX_IM:
		{
			cpu->x--;
			cycles--;
			SetStatusFlags(cpu, cpu->x);
		} break;
		case DEY_IM:
		{
			cpu->y--;
			cycles--;
			SetStatusFlags(cpu, cpu->y);
		} break;
		case BEQ:
		{
			byte offset = FetchByte(cpu, mem, &cycles);
			if (cpu->Flags[zeroFlag])
			{
				const word PCold = cpu->pc;
				cpu->pc += offset;
				cycles--;

				const bool PageChanged = (cpu->pc >> 8) != (PCold >> 8);
				if (PageChanged)
				{
					cycles -= 2;
				}
			}
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
		case JMP_ABS:
		{
			word Address = FetchWord(cpu, mem, &cycles);
			cpu->pc = Address;
		} break;
		case JMP_IND:
		{
			word Address = FetchWord(cpu, mem, &cycles);
			Address = ReadWord(mem, Address, &cycles);
			cpu->pc = Address;
		} break;
		default:
		{
			printf("Instruction not handled %d\n", Instruction);
		} break;
		}
	}

	return numCycles - cycles;
}