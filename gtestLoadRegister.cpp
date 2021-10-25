#include "gtest/gtest.h"
#include "6502.h"


struct CPU gtestLoadcpu;
struct memory gtestLoadmem;


static void CheckStatusFlag(const struct CPU& cpu1, const struct CPU& cpu2)
{
	EXPECT_FALSE(cpu1.Flags[zeroFlag]);
	EXPECT_FALSE(cpu1.Flags[negativeFlag - 1]);
	EXPECT_EQ(cpu1.Flags[interruptDisable], cpu2.Flags[interruptDisable]);
	EXPECT_EQ(cpu1.Flags[decimalMode], cpu2.Flags[decimalMode]);
	EXPECT_EQ(cpu1.Flags[breakCommand], cpu2.Flags[breakCommand]);
	EXPECT_EQ(cpu1.Flags[overflowFlag], cpu2.Flags[overflowFlag]);
}

TEST(testLoadReg, CPUcyclesNeg)
{
	const uint32_t cycles = 0;
	uint32_t numCycles = Execute(&gtestLoadcpu, &gtestLoadmem, cycles);
	EXPECT_EQ(numCycles, 0);
}

TEST(testLoadReg, LDA_IM_TEST_ZERO_FLAG)
{
	ResetCpu(&gtestLoadcpu, &gtestLoadmem);

	gtestLoadcpu.acc = 0x3;
	gtestLoadmem.Data[0xFFFC] = LDA_IM;
	gtestLoadmem.Data[0xFFFD] = 0x0;

	Execute(&gtestLoadcpu, &gtestLoadmem, 2);

	EXPECT_EQ(gtestLoadcpu.acc, 0x0);
	EXPECT_TRUE(gtestLoadcpu.Flags[zeroFlag]);
}


static void gtestLoadRegisterIM(struct CPU* cpu, struct memory* mem, byte const opcode, byte* const reg)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x80;

	uint32_t numCycles = Execute(cpu, mem, 2);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*reg, 0x80);
	EXPECT_EQ(numCycles, 2);
	CheckStatusFlag(*cpu, cpuCopy);
}

static void gtestLoadRegisterZP(struct CPU* cpu, struct memory* mem, byte const opcode, byte* const reg)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x10;
	mem->Data[0x10] = 0x35;
	uint32_t numCycles = Execute(cpu, mem, 3);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*reg, 0x35);
	EXPECT_EQ(numCycles, 3);
	CheckStatusFlag(*cpu, cpuCopy);
}


static void gtestLoadRegisterZPX(struct CPU* cpu, struct memory* mem, byte const opcode, byte* const reg)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->x = 5;
	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x42;
	mem->Data[0x42 + cpu->x] = 0x37;

	uint32_t numCycles = Execute(cpu, mem, 4);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*reg, 0x37);
	EXPECT_EQ(numCycles, 4);
	CheckStatusFlag(*cpu, cpuCopy);
}

static void gtestLoadRegisterZPY(struct CPU* cpu, struct memory* mem, byte const opcode, byte* const reg)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->y = 5;
	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x42;
	mem->Data[0x42 + cpu->y] = 0x37;

	uint32_t numCycles = Execute(cpu, mem, 4);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*reg, 0x37);
	EXPECT_EQ(numCycles, 4);
	CheckStatusFlag(*cpu, cpuCopy);
}

static void gtestLoadRegisterAbsolute(struct CPU* cpu, struct memory* mem, byte const opcode, byte* const reg)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x90;
	mem->Data[0xFFFE] = 0x32;
	mem->Data[0x3290] = 0x27;

	const uint32_t ExpectedCycles = 4;

	uint32_t numCycles = Execute(cpu, mem, ExpectedCycles);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*reg, 0x27);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(*cpu, cpuCopy);
}


static void gtestLoadRegisterAbsoluteX(struct CPU* cpu, struct memory* mem, byte const opcode, byte* const reg)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->x = 0x5;

	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x90;
	mem->Data[0xFFFE] = 0x40;
	mem->Data[0x4090 + cpu->x] = 0x37;

	const uint32_t ExpectedCycles = 4;

	uint32_t numCycles = Execute(cpu, mem, ExpectedCycles);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*reg, 0x37);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(*cpu, cpuCopy);
}

static void gtestLoadRegisterAbsoluteY(struct CPU* cpu, struct memory* mem, byte const opcode, byte* const	reg)
{
	ResetCpu(cpu, mem);
	cpu->Flags[negativeFlag - 1] = cpu->Flags[zeroFlag] = true;

	cpu->y = 0x5;

	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x90;
	mem->Data[0xFFFE] = 0x40;
	mem->Data[0x4090 + cpu->y] = 0x37;

	const uint32_t ExpectedCycles = 4;

	uint32_t numCycles = Execute(cpu, mem, ExpectedCycles);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*reg, 0x37);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(*cpu, cpuCopy);
}

TEST(testLoadReg, LDA_IM_TEST)									// LDA immidiately
{
	gtestLoadRegisterIM(&gtestLoadcpu, &gtestLoadmem, LDA_IM, &gtestLoadcpu.acc);
}

TEST(testLoadReg, LDX_IM_TEST)									// LDX immidiately
{
	gtestLoadRegisterIM(&gtestLoadcpu, &gtestLoadmem, LDX_IM, &gtestLoadcpu.x);
}

TEST(testLoadReg, LDY_IM_TEST)									// LDY immidiately
{
	gtestLoadRegisterIM(&gtestLoadcpu, &gtestLoadmem, LDY_IM, &gtestLoadcpu.y);
}

TEST(testLoadReg, LDA_ZP_TEST)									// LDA zero page
{
	gtestLoadRegisterZP(&gtestLoadcpu, &gtestLoadmem, LDA_ZP, &gtestLoadcpu.acc);
}

TEST(testLoadReg, LDX_ZP_TEST)									// LDX zero page
{
	gtestLoadRegisterZP(&gtestLoadcpu, &gtestLoadmem, LDX_ZP, &gtestLoadcpu.x);
}

TEST(testLoadReg, LDY_ZP_TEST)									// LDY zero page
{
	gtestLoadRegisterZP(&gtestLoadcpu, &gtestLoadmem, LDY_ZP, &gtestLoadcpu.y);
}

TEST(testLoadReg, LDA_ZPX_TEST)								// LDA zero page + x register
{
	gtestLoadRegisterZPX(&gtestLoadcpu, &gtestLoadmem, LDA_ZPX, &gtestLoadcpu.acc);
}

TEST(testLoadReg, LDX_ZPY_TEST)								// LDX zero page + y register
{
	gtestLoadRegisterZPY(&gtestLoadcpu, &gtestLoadmem, LDX_ZPY, &gtestLoadcpu.x);
}

TEST(testLoadReg, LDY_ZPX_TEST)								// LDY zero page + x register
{
	gtestLoadRegisterZPX(&gtestLoadcpu, &gtestLoadmem, LDY_ZPX, &gtestLoadcpu.y);
}

TEST(testLoadReg, LDA_ZPX_WRAP_TEST)
{
	ResetCpu(&gtestLoadcpu, &gtestLoadmem);

	gtestLoadcpu.x = 0xFF;
	gtestLoadmem.Data[0xFFFC] = LDA_ZPX;
	gtestLoadmem.Data[0xFFFD] = 0x80;
	gtestLoadmem.Data[0x7F] = 0x37;

	uint32_t numCycles = Execute(&gtestLoadcpu, &gtestLoadmem, 4);
	const struct CPU cpuCopy = gtestLoadcpu;

	EXPECT_EQ(gtestLoadcpu.acc, 0x37);
	EXPECT_EQ(numCycles, 4);
	CheckStatusFlag(gtestLoadcpu, cpuCopy);
}


TEST(testLoadReg, LDA_ABS_TEST)								// LDA absolute
{
	gtestLoadRegisterAbsolute(&gtestLoadcpu, &gtestLoadmem, LDA_ABS, &gtestLoadcpu.acc);
}

TEST(testLoadReg, LDX_ABS_TEST)								// LDX absolute
{
	gtestLoadRegisterAbsolute(&gtestLoadcpu, &gtestLoadmem, LDX_ABS, &gtestLoadcpu.x);
}

TEST(testLoadReg, LDY_ABS_TEST)								// LDY absolute
{
	gtestLoadRegisterAbsolute(&gtestLoadcpu, &gtestLoadmem, LDY_ABS, &gtestLoadcpu.y);
}


TEST(testLoadReg, LDA_ABSX_TEST)							// LDA absolute + x register
{
	gtestLoadRegisterAbsoluteX(&gtestLoadcpu, &gtestLoadmem, LDA_ABSX, &gtestLoadcpu.acc);
}

TEST(testLoadReg, LDA_ABSY_TEST)							// LDA absolute + y register
{
	gtestLoadRegisterAbsoluteY(&gtestLoadcpu, &gtestLoadmem, LDA_ABSY, &gtestLoadcpu.acc);
}

TEST(testLoadReg, LDX_ABSX_TEST)							// LDX absolute + y register
{
	gtestLoadRegisterAbsoluteY(&gtestLoadcpu, &gtestLoadmem, LDX_ABSY, &gtestLoadcpu.x);
}

TEST(testLoadReg, LDY_ABSX_TEST)							// LDY absolute + x register
{
	gtestLoadRegisterAbsoluteX(&gtestLoadcpu, &gtestLoadmem, LDY_ABSX, &gtestLoadcpu.y);
}


TEST(testLoadReg, LDA_INDX_TEST)
{
	ResetCpu(&gtestLoadcpu, &gtestLoadmem);
	gtestLoadcpu.x = 0x04;

	gtestLoadmem.Data[0xFFFC] = LDA_INDX;
	gtestLoadmem.Data[0xFFFD] = 0x02;
	gtestLoadmem.Data[0x06] = 0x00;
	gtestLoadmem.Data[0x07] = 0x80;
	gtestLoadmem.Data[0x8000] = 0x37;

	const uint32_t ExpectedCycles = 6;

	uint32_t numCycles = Execute(&gtestLoadcpu, &gtestLoadmem, ExpectedCycles);
	const struct CPU cpuCopy = gtestLoadcpu;

	EXPECT_EQ(gtestLoadcpu.acc, 0x37);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(gtestLoadcpu, cpuCopy);
}

TEST(testLoadReg, LDA_INDY_TEST)
{
	ResetCpu(&gtestLoadcpu, &gtestLoadmem);
	gtestLoadcpu.y = 0x04;

	gtestLoadmem.Data[0xFFFC] = LDA_INDY;
	gtestLoadmem.Data[0xFFFD] = 0x02;
	gtestLoadmem.Data[0x02] = 0x00;
	gtestLoadmem.Data[0x03] = 0x80;
	gtestLoadmem.Data[0x8000 + gtestLoadcpu.y] = 0x37;

	const uint32_t ExpectedCycles = 5;

	uint32_t numCycles = Execute(&gtestLoadcpu, &gtestLoadmem, ExpectedCycles);
	const struct CPU cpuCopy = gtestLoadcpu;

	EXPECT_EQ(gtestLoadcpu.acc, 0x37);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(gtestLoadcpu, cpuCopy);
}

TEST(testLoadReg, LDA_INDY_BOUNDARY_TEST)
{
	ResetCpu(&gtestLoadcpu, &gtestLoadmem);
	gtestLoadcpu.y = 0xFF;

	gtestLoadmem.Data[0xFFFC] = LDA_INDY;
	gtestLoadmem.Data[0xFFFD] = 0x02;
	gtestLoadmem.Data[0x02] = 0x00;
	gtestLoadmem.Data[0x03] = 0x80;
	gtestLoadmem.Data[0x8000 + gtestLoadcpu.y] = 0x37;

	const uint32_t ExpectedCycles = 6;

	uint32_t numCycles = Execute(&gtestLoadcpu, &gtestLoadmem, ExpectedCycles);
	const struct CPU cpuCopy = gtestLoadcpu;

	EXPECT_EQ(gtestLoadcpu.acc, 0x37);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(gtestLoadcpu, cpuCopy);
}