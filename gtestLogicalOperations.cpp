#include "gtest/gtest.h"
#include "6502.h"


struct CPU gtestLogcpu;
struct memory gtestLogmem;

enum ELogicalOP
{
	AND, 
	EOR, 
	OR
};

static byte LogicalOP(byte a, byte b, ELogicalOP opcode, struct memory* mem)						// test for logical operations(and, or, eor)
{
	switch (opcode)
	{
	case ELogicalOP::AND:
		return a & b;
		break;
	case ELogicalOP::EOR:
		return a ^ b;
		break;
	case ELogicalOP::OR:
		return a | b;
		break;
	}
}

static void CheckStatusFlag(const struct CPU& cpu1, const struct CPU& cpu2)
{
	EXPECT_EQ(cpu1.Flags[zeroFlag], cpu2.Flags[zeroFlag]);
	EXPECT_EQ(cpu1.Flags[negativeFlag - 1], cpu2.Flags[negativeFlag - 1]);
	EXPECT_EQ(cpu1.Flags[interruptDisable], cpu2.Flags[interruptDisable]);
	EXPECT_EQ(cpu1.Flags[decimalMode], cpu2.Flags[decimalMode]);
	EXPECT_EQ(cpu1.Flags[breakCommand], cpu2.Flags[breakCommand]);
	EXPECT_EQ(cpu1.Flags[overflowFlag], cpu2.Flags[overflowFlag]);
}


static void gtestLogicalOnARegisterIM(struct CPU* cpu, struct memory* mem, ELogicalOP opcode)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->acc = 0xCC;

	switch (opcode)
	{
	case ELogicalOP::AND:
		mem->Data[0xFFFC] = AND_IM;
		break;
	case ELogicalOP::EOR:
		mem->Data[0xFFFC] = EOR_IM;
		break;
	case ELogicalOP::OR:
		mem->Data[0xFFFC] = OR_IM;
		break;
	}
	mem->Data[0xFFFD] = 0x80;

	uint32_t numCycles = Execute(cpu, mem, 2);
	const struct CPU cpuCopy = *cpu;

	const byte ExpectedResult = LogicalOP(0xCC, 0x80, opcode, mem);

	EXPECT_EQ(cpu->acc, ExpectedResult);
	EXPECT_EQ(numCycles, 2);

	EXPECT_FALSE(cpu->Flags[zeroFlag]);

	CheckStatusFlag(*cpu, cpuCopy);
}

static void gtestLogicalRegisterZP(struct CPU* cpu, struct memory* mem, ELogicalOP opcode)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->acc = 0xCC;

	switch (opcode)
	{
	case ELogicalOP::AND:
		mem->Data[0xFFFC] = AND_ZP;
		break;
	case ELogicalOP::EOR:
		mem->Data[0xFFFC] = EOR_ZP;
		break;
	case ELogicalOP::OR:
		mem->Data[0xFFFC] = OR_ZP;
		break;
	}
	mem->Data[0xFFFD] = 0x80;
	mem->Data[0x0080] = 0x60;

	uint32_t numCycles = Execute(cpu, mem, 3);
	const struct CPU cpuCopy = *cpu;

	const byte ExpectedResult = LogicalOP(0xCC, 0x60, opcode, mem);

	EXPECT_EQ(cpu->acc, ExpectedResult);
	EXPECT_EQ(numCycles, 3);

	EXPECT_FALSE(cpu->Flags[zeroFlag]);

	CheckStatusFlag(*cpu, cpuCopy);
}


static void gtestLogicalRegisterZPX(struct CPU* cpu, struct memory* mem, ELogicalOP opcode)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->acc = 0xCC;
	cpu->x = 5;

	switch (opcode)
	{
	case ELogicalOP::AND:
		mem->Data[0xFFFC] = AND_ZPX;
		break;
	case ELogicalOP::EOR:
		mem->Data[0xFFFC] = EOR_ZPX;
		break;
	case ELogicalOP::OR:
		mem->Data[0xFFFC] = OR_ZPX;
		break;
	}
	mem->Data[0xFFFD] = 0x80;
	mem->Data[0x0080 + cpu->x] = 0x60;

	uint32_t numCycles = Execute(cpu, mem, 3);
	const struct CPU cpuCopy = *cpu;

	const byte ExpectedResult = LogicalOP(0xCC, 0x60, opcode, mem);

	EXPECT_EQ(cpu->acc, ExpectedResult);
	EXPECT_EQ(numCycles, 3);

	EXPECT_FALSE(cpu->Flags[zeroFlag]);

	CheckStatusFlag(*cpu, cpuCopy);
}

static void gtestLogicalRegisterAbsolute(struct CPU* cpu, struct memory* mem, ELogicalOP opcode)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->acc = 0xCC;

	switch (opcode)
	{
	case ELogicalOP::AND:
		mem->Data[0xFFFC] = AND_ABS;
		break;
	case ELogicalOP::EOR:
		mem->Data[0xFFFC] = EOR_ABS;
		break;
	case ELogicalOP::OR:
		mem->Data[0xFFFC] = OR_ABS;
		break;
	}
	mem->Data[0xFFFD] = 0x80;
	mem->Data[0xFFFE] = 0x32;
	mem->Data[0x3280] = 0x60;

	uint32_t numCycles = Execute(cpu, mem, 4);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(cpu->acc, LogicalOP(0xCC, 0x60, opcode, mem));
	EXPECT_EQ(numCycles, 4);
	EXPECT_FALSE(cpu->Flags[zeroFlag]);
}


static void gtestLogicalRegisterAbsoluteX(struct CPU* cpu, struct memory* mem, ELogicalOP opcode)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->acc = 0xCC;
	cpu->x = 5;

	switch (opcode)
	{
	case ELogicalOP::AND:
		mem->Data[0xFFFC] = AND_ABSX;
		break;
	case ELogicalOP::EOR:
		mem->Data[0xFFFC] = EOR_ABSX;
		break;
	case ELogicalOP::OR:
		mem->Data[0xFFFC] = OR_ABSX;
		break;
	}
	mem->Data[0xFFFD] = 0x80;
	mem->Data[0xFFFE] = 0x32;
	mem->Data[0x3280 + cpu->x] = 0x60;

	uint32_t numCycles = Execute(cpu, mem, 4);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(cpu->acc, LogicalOP(0xCC, 0x60, opcode, mem));
	EXPECT_EQ(numCycles, 4);
	EXPECT_FALSE(cpu->Flags[zeroFlag]);
}

static void gtestLogicalRegisterAbsoluteY(struct CPU* cpu, struct memory* mem, ELogicalOP opcode)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->acc = 0xCC;
	cpu->y = 5;

	switch (opcode)
	{
	case ELogicalOP::AND:
		mem->Data[0xFFFC] = AND_ABSY;
		break;
	case ELogicalOP::EOR:
		mem->Data[0xFFFC] = EOR_ABSY;
		break;
	case ELogicalOP::OR:
		mem->Data[0xFFFC] = OR_ABSY;
		break;
	}
	mem->Data[0xFFFD] = 0x80;
	mem->Data[0xFFFE] = 0x32;
	mem->Data[0x3280 + cpu->y] = 0x60;

	uint32_t numCycles = Execute(cpu, mem, 4);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(cpu->acc, LogicalOP(0xCC, 0x60, opcode, mem));
	EXPECT_EQ(numCycles, 4);
	EXPECT_FALSE(cpu->Flags[zeroFlag]);
}

static void gtestLogicalRegisterINDX(struct CPU* cpu, struct memory* mem, ELogicalOP opcode)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->acc = 0xCC;
	cpu->x = 5;

	switch (opcode)
	{
	case ELogicalOP::AND:
		mem->Data[0xFFFC] = AND_INDX;
		break;
	case ELogicalOP::EOR:
		mem->Data[0xFFFC] = EOR_INDX;
		break;
	case ELogicalOP::OR:
		mem->Data[0xFFFC] = OR_INDX;
		break;
	}
	mem->Data[0xFFFD] = 0x32;
	mem->Data[0x32 + cpu->x] = 0x80;
	mem->Data[0x33 + cpu->x] = 0x00;
	mem->Data[0x0080] = 0x60;

	uint32_t numCycles = Execute(cpu, mem, 7);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(cpu->acc, LogicalOP(0xCC, 0x60, opcode, mem));
	EXPECT_EQ(numCycles, 7);
	EXPECT_FALSE(cpu->Flags[zeroFlag]);
}


static void gtestLogicalRegisterINDY(struct CPU* cpu, struct memory* mem, ELogicalOP opcode)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->acc = 0xCC;
	cpu->y = 5;

	switch (opcode)
	{
	case ELogicalOP::AND:
		mem->Data[0xFFFC] = AND_INDY;
		break;
	case ELogicalOP::EOR:
		mem->Data[0xFFFC] = EOR_INDY;
		break;
	case ELogicalOP::OR:
		mem->Data[0xFFFC] = OR_INDY;
		break;
	}
	mem->Data[0xFFFD] = 0x32;
	mem->Data[0x32] = 0x80;
	mem->Data[0x33] = 0x00;
	mem->Data[0x0080 + cpu->y] = 0x60;

	uint32_t numCycles = Execute(cpu, mem, 5);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(cpu->acc, LogicalOP(0xCC, 0x60, opcode, mem));
	EXPECT_EQ(numCycles, 5);
	EXPECT_FALSE(cpu->Flags[zeroFlag]);
}


TEST(testLogOperations, AND_IM_TEST)									// AND immidiately
{
	gtestLogicalOnARegisterIM(&gtestLogcpu, &gtestLogmem, ELogicalOP::AND);
}

TEST(testLogOperations, EOR_IM_TEST)									// EOR immidiately
{
	gtestLogicalOnARegisterIM(&gtestLogcpu, &gtestLogmem, ELogicalOP::EOR);
}

TEST(testLogOperations, OR_IM_TEST)									// OR immidiately
{
	gtestLogicalOnARegisterIM(&gtestLogcpu, &gtestLogmem, ELogicalOP::OR);
}

TEST(testLogOperations, AND_ZP_TEST)									// AND zero page
{
	gtestLogicalRegisterZP(&gtestLogcpu, &gtestLogmem, ELogicalOP::AND);
}

TEST(testLogOperations, OR_ZP_TEST)									// OR zero page
{
	gtestLogicalRegisterZP(&gtestLogcpu, &gtestLogmem, ELogicalOP::OR);
}

TEST(testLogOperations, EOR_ZP_TEST)									// EOR zero page
{
	gtestLogicalRegisterZP(&gtestLogcpu, &gtestLogmem, ELogicalOP::EOR);
}

TEST(testLogOperations, AND_ZPX_TEST)									// AND zero page + x register
{
	gtestLogicalRegisterZPX(&gtestLogcpu, &gtestLogmem, ELogicalOP::AND);
}

TEST(testLogOperations, OR_ZPX_TEST)									// OR zero page + x register
{
	gtestLogicalRegisterZPX(&gtestLogcpu, &gtestLogmem, ELogicalOP::OR);
}

TEST(testLogOperations, EOR_ZPX_TEST)									// EOR zero page + x register
{
	gtestLogicalRegisterZPX(&gtestLogcpu, &gtestLogmem, ELogicalOP::EOR);
}

TEST(testLogOperations, AND_ABS_TEST)									// AND absolute
{
	gtestLogicalRegisterAbsolute(&gtestLogcpu, &gtestLogmem, ELogicalOP::AND);
}

TEST(testLogOperations, OR_ABS_TEST)									// OR absolute
{
	gtestLogicalRegisterAbsolute(&gtestLogcpu, &gtestLogmem, ELogicalOP::OR);
}

TEST(testLogOperations, EOR_ABS_TEST)									// EOR absolute
{
	gtestLogicalRegisterAbsolute(&gtestLogcpu, &gtestLogmem, ELogicalOP::EOR);
}

TEST(testLogOperations, AND_ABSX_TEST)									// AND absolute + x register
{
	gtestLogicalRegisterAbsoluteX(&gtestLogcpu, &gtestLogmem, ELogicalOP::AND);
}

TEST(testLogOperations, OR_ABSX_TEST)									// OR absolute + x register
{
	gtestLogicalRegisterAbsoluteX(&gtestLogcpu, &gtestLogmem, ELogicalOP::OR);
}

TEST(testLogOperations, EOR_ABSX_TEST)									// EOR absolute + x register
{
	gtestLogicalRegisterAbsoluteX(&gtestLogcpu, &gtestLogmem, ELogicalOP::EOR);
}

TEST(testLogOperations, AND_ABSY_TEST)									// AND absolute + y register
{
	gtestLogicalRegisterAbsoluteY(&gtestLogcpu, &gtestLogmem, ELogicalOP::AND);
}

TEST(testLogOperations, OR_ABSY_TEST)									// OR absolute + y register
{
	gtestLogicalRegisterAbsoluteY(&gtestLogcpu, &gtestLogmem, ELogicalOP::OR);
}

TEST(testLogOperations, EOR_ABSY_TEST)									// EOR absolute + y register
{
	gtestLogicalRegisterAbsoluteY(&gtestLogcpu, &gtestLogmem, ELogicalOP::EOR);
}


TEST(testLogOperations, AND_INDX_TEST)									// AND indx
{
	gtestLogicalRegisterINDX(&gtestLogcpu, &gtestLogmem, ELogicalOP::AND);
}

TEST(testLogOperations, OR_INDX_TEST)									// OR indx
{
	gtestLogicalRegisterINDX(&gtestLogcpu, &gtestLogmem, ELogicalOP::OR);
}

TEST(testLogOperations, EOR_INDX_TEST)									// EOR indx
{
	gtestLogicalRegisterINDX(&gtestLogcpu, &gtestLogmem, ELogicalOP::EOR);
}

TEST(testLogOperations, AND_INDY_TEST)									// AND indy
{	
	gtestLogicalRegisterINDY(&gtestLogcpu, &gtestLogmem, ELogicalOP::AND);
}

TEST(testLogOperations, OR_INDY_TEST)									// OR indy
{
	gtestLogicalRegisterINDY(&gtestLogcpu, &gtestLogmem, ELogicalOP::OR);
}

TEST(testLogOperations, EOR_INDY_TEST)									// EOR indy
{
	gtestLogicalRegisterINDY(&gtestLogcpu, &gtestLogmem, ELogicalOP::EOR);
}