#include "gtest/gtest.h"
#include "6502.h"

struct CPU gtestStorecpu;
struct memory gtestStoremem;


static void CheckStatusFlag(const struct CPU& cpu1, const struct CPU& cpu2)
{
	EXPECT_EQ(cpu1.Flags[zeroFlag], cpu2.Flags[zeroFlag]);
	EXPECT_EQ(cpu1.Flags[negativeFlag - 1], cpu2.Flags[negativeFlag - 1]);
	EXPECT_EQ(cpu1.Flags[interruptDisable], cpu2.Flags[interruptDisable]);
	EXPECT_EQ(cpu1.Flags[decimalMode], cpu2.Flags[decimalMode]);
	EXPECT_EQ(cpu1.Flags[breakCommand], cpu2.Flags[breakCommand]);
	EXPECT_EQ(cpu1.Flags[overflowFlag], cpu2.Flags[overflowFlag]);
}

static void gtestStoreRegisterZP(struct CPU* cpu, struct memory* mem, byte const opcode, byte* reg)
{
	ResetCpu(cpu, mem);

	*reg = 0xA;

	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x80;
	mem->Data[0x0080] = 0x00;

	uint32_t numCycles = Execute(cpu, mem, 3);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(mem->Data[0x0080], *reg);
	EXPECT_EQ(numCycles, 3);
	CheckStatusFlag(*cpu, cpuCopy);
}


static void gtestStoreRegisterZPX(struct CPU* cpu, struct memory* mem, byte const opcode, byte* reg)
{
	ResetCpu(cpu, mem);

	*reg = 0xA;

	cpu->x = 0x5;
	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x42;
	mem->Data[0x42 + cpu->x] = 0x00;

	uint32_t numCycles = Execute(cpu, mem, 4);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(mem->Data[0x42 + cpu->x], *reg);
	EXPECT_EQ(numCycles, 4);
	CheckStatusFlag(*cpu, cpuCopy);
}

static void gtestStoreRegisterZPY(struct CPU* cpu, struct memory* mem, byte const opcode, byte* reg)
{
	ResetCpu(cpu, mem);

	*reg = 0xA;

	cpu->y = 0x5;
	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x42;
	mem->Data[0x42 + cpu->y] = 0x00;

	uint32_t numCycles = Execute(cpu, mem, 4);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(mem->Data[0x42 + cpu->y], *reg);
	EXPECT_EQ(numCycles, 4);
	CheckStatusFlag(*cpu, cpuCopy);
}



static void gtestStoreRegisterAbsolute(struct CPU* cpu, struct memory* mem, byte const opcode, byte* reg)
{
	ResetCpu(cpu, mem);

	*reg = 0xA;

	mem->Data[0xFFFC] = opcode;
	mem->Data[0xFFFD] = 0x90;
	mem->Data[0xFFFE] = 0x32;
	mem->Data[0x3290] = 0x00;

	const uint32_t ExpectedCycles = 5;

	uint32_t numCycles = Execute(cpu, mem, ExpectedCycles);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(mem->Data[0x3290], *reg);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(*cpu, cpuCopy);
}


TEST(testStoreReg, STA_ZP_TEST)						// STA zero page
{
	gtestStoreRegisterZP(&gtestStorecpu, &gtestStoremem, STA_ZP, &gtestStorecpu.acc);
}

TEST(testStoreReg, STA_ZPX_TEST)					// STA zero page + x register
{
	gtestStoreRegisterZPX(&gtestStorecpu, &gtestStoremem, STA_ZPX, &gtestStorecpu.acc);
}

TEST(testStoreReg, STA_ABS_TEST)					// STA absolute
{
	gtestStoreRegisterAbsolute(&gtestStorecpu, &gtestStoremem, STA_ABS, &gtestStorecpu.acc);
}

TEST(testStoreReg, STA_ABSX_TEST)					// STA absolute + x register
{
	ResetCpu(&gtestStorecpu, &gtestStoremem);

	gtestStorecpu.acc = 0xA;
	gtestStorecpu.x = 0x5;

	gtestStoremem.Data[0xFFFC] = STA_ABSX;
	gtestStoremem.Data[0xFFFD] = 0x00;
	gtestStoremem.Data[0xFFFE] = 0x80;
	gtestStoremem.Data[0x8000 + gtestStorecpu.x] = 0x0;

	const uint32_t ExpectedCycles = 5;

	uint32_t numCycles = Execute(&gtestStorecpu, &gtestStoremem, ExpectedCycles);
	const struct CPU cpuCopy = gtestStorecpu;

	EXPECT_EQ(gtestStoremem.Data[0x8000 + gtestStorecpu.x], 0xA);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(gtestStorecpu, cpuCopy);
}

TEST(testStoreReg, STA_ABSY_TEST)					// STA absolute + y register
{
	ResetCpu(&gtestStorecpu, &gtestStoremem);

	gtestStorecpu.acc = 0xA;
	gtestStorecpu.y = 0x5;

	gtestStoremem.Data[0xFFFC] = STA_ABSY;
	gtestStoremem.Data[0xFFFD] = 0x00;
	gtestStoremem.Data[0xFFFE] = 0x80;
	gtestStoremem.Data[0x8000 + gtestStorecpu.y] = 0x0;

	const uint32_t ExpectedCycles = 5;

	uint32_t numCycles = Execute(&gtestStorecpu, &gtestStoremem, ExpectedCycles);
	const struct CPU cpuCopy = gtestStorecpu;

	EXPECT_EQ(gtestStoremem.Data[0x8005], 0xA);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(gtestStorecpu, cpuCopy);
}

TEST(testStoreReg, STX_ZP_TEST)						// STX zero page
{
	gtestStoreRegisterZP(&gtestStorecpu, &gtestStoremem, STX_ZP, &gtestStorecpu.x);
}

TEST(testStoreReg, STX_ZPY_TEST)					// STX zero page + y register
{
	gtestStoreRegisterZP(&gtestStorecpu, &gtestStoremem, STX_ZPY, &gtestStorecpu.x);
}

TEST(testStoreReg, STX_ABS_TEST)					// STX absolute
{
	gtestStoreRegisterAbsolute(&gtestStorecpu, &gtestStoremem, STX_ABS, &gtestStorecpu.x);
}

TEST(testStoreReg, STY_ZP_TEST)						// STY zero page
{
	gtestStoreRegisterZP(&gtestStorecpu, &gtestStoremem, STY_ZP, &gtestStorecpu.y);
}

TEST(testStoreReg, STY_ZPX_TEST)					// STY zero page + x register
{
	gtestStoreRegisterZPX(&gtestStorecpu, &gtestStoremem, STY_ZPX, &gtestStorecpu.y);
}

TEST(testStoreReg, STY_ABS_TEST)					// STY absolute
{
	gtestStoreRegisterAbsolute(&gtestStorecpu, &gtestStoremem, STY_ABS, &gtestStorecpu.y);
}


TEST(testStoreReg, STA_INDX_TEST)					// STA indirect x
{
	ResetCpu(&gtestStorecpu, &gtestStoremem);

	gtestStorecpu.acc = 0xA;
	gtestStorecpu.x = 0x5;

	gtestStoremem.Data[0xFFFC] = STA_INDX;
	gtestStoremem.Data[0xFFFD] = 0x20;
	gtestStoremem.Data[0x0020 + gtestStorecpu.x] = 0x80;
	gtestStoremem.Data[0x0021 + gtestStorecpu.x] = 0x00;
	gtestStoremem.Data[0x8000] = 0x0;

	const uint32_t ExpectedCycles = 6;

	uint32_t numCycles = Execute(&gtestStorecpu, &gtestStoremem, ExpectedCycles);
	const struct CPU cpuCopy = gtestStorecpu;

	EXPECT_EQ(gtestStoremem.Data[0x0080], 0xA);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(gtestStorecpu, cpuCopy);
}


TEST(testStoreReg, STA_INDY_TEST)					// STA indirect y
{
	ResetCpu(&gtestStorecpu, &gtestStoremem);

	gtestStorecpu.acc = 0xA;
	gtestStorecpu.y = 0x5;

	gtestStoremem.Data[0xFFFC] = STA_INDX;
	gtestStoremem.Data[0xFFFD] = 0x20;
	gtestStoremem.Data[0x0020] = 0x00;
	gtestStoremem.Data[0x0021] = 0x80;
	gtestStoremem.Data[0x8000 + gtestStorecpu.y] = 0x00;

	const uint32_t ExpectedCycles = 6;

	uint32_t numCycles = Execute(&gtestStorecpu, &gtestStoremem, ExpectedCycles);
	const struct CPU cpuCopy = gtestStorecpu;

	EXPECT_EQ(gtestStoremem.Data[0x8000 + gtestStorecpu.x], 0xA);
	EXPECT_EQ(numCycles, ExpectedCycles);
	CheckStatusFlag(gtestStorecpu, cpuCopy);
}