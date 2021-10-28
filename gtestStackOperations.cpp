#include "gtest/gtest.h"
#include "6502.h"

struct CPU gtestStackInstrcpu;
struct memory gtestStackInstrmem;

static void CheckStatusFlag(const struct CPU& cpu1, const struct CPU& cpu2)
{
	EXPECT_EQ(cpu1.Flags[zeroFlag], cpu2.Flags[zeroFlag]);
	EXPECT_EQ(cpu1.Flags[negativeFlag - 1], cpu2.Flags[negativeFlag - 1]);
	EXPECT_EQ(cpu1.Flags[interruptDisable], cpu2.Flags[interruptDisable]);
	EXPECT_EQ(cpu1.Flags[decimalMode], cpu2.Flags[decimalMode]);
	EXPECT_EQ(cpu1.Flags[breakCommand], cpu2.Flags[breakCommand]);
	EXPECT_EQ(cpu1.Flags[overflowFlag], cpu2.Flags[overflowFlag]);
}


TEST(testJumpInstructions, TSX_TEST)				// TSX
{
	ResetCpu(&gtestStackInstrcpu, &gtestStackInstrmem);
	gtestStackInstrcpu.pc = 0xFF00;

	gtestStackInstrcpu.Flags[negativeFlag - 1] = gtestStackInstrcpu.Flags[zeroFlag] = true;

	gtestStackInstrcpu.x = 0x00;
	gtestStackInstrcpu.sp = 0x01;

	gtestStackInstrmem.Data[0xFF00] = TSX;
	
	const uint32_t ExpectedCycles = 2;

	uint32_t numCycles = Execute(&gtestStackInstrcpu, &gtestStackInstrmem, ExpectedCycles);

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestStackInstrcpu.x, 0x01);
	EXPECT_FALSE(gtestStackInstrcpu.Flags[negativeFlag - 1]);
	EXPECT_FALSE(gtestStackInstrcpu.Flags[zeroFlag]);
}


TEST(testJumpInstructions, TSX_ZERO_FLAGTEST)		// TSX zero flag test
{
	ResetCpu(&gtestStackInstrcpu, &gtestStackInstrmem);
	gtestStackInstrcpu.pc = 0xFF00;

	gtestStackInstrcpu.Flags[negativeFlag - 1] = gtestStackInstrcpu.Flags[zeroFlag] = true;

	gtestStackInstrcpu.x = 0x00;
	gtestStackInstrcpu.sp = 0x00;

	gtestStackInstrmem.Data[0xFF00] = TSX;

	const uint32_t ExpectedCycles = 2;

	uint32_t numCycles = Execute(&gtestStackInstrcpu, &gtestStackInstrmem, ExpectedCycles);

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestStackInstrcpu.x, 0x00);
	EXPECT_FALSE(gtestStackInstrcpu.Flags[negativeFlag - 1]);
	EXPECT_TRUE(gtestStackInstrcpu.Flags[zeroFlag]);
}


TEST(testJumpInstructions, TSX_NEG_FLAGTEST)		// TSX negtive flag test
{
	ResetCpu(&gtestStackInstrcpu, &gtestStackInstrmem);
	gtestStackInstrcpu.pc = 0xFF00;

	gtestStackInstrcpu.Flags[negativeFlag - 1] = gtestStackInstrcpu.Flags[zeroFlag] = true;

	gtestStackInstrcpu.x = 0x00;
	gtestStackInstrcpu.sp = 0x80;

	gtestStackInstrmem.Data[0xFF00] = TSX;

	const uint32_t ExpectedCycles = 2;

	uint32_t numCycles = Execute(&gtestStackInstrcpu, &gtestStackInstrmem, ExpectedCycles);

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestStackInstrcpu.x, 0x80);
	EXPECT_TRUE(gtestStackInstrcpu.Flags[negativeFlag - 1]);
	EXPECT_FALSE(gtestStackInstrcpu.Flags[zeroFlag]);
}


TEST(testJumpInstructions, TXS_TEST)				// TXS
{
	ResetCpu(&gtestStackInstrcpu, &gtestStackInstrmem);
	gtestStackInstrcpu.pc = 0xFF00;

	gtestStackInstrcpu.x = 0xFF;
	gtestStackInstrcpu.sp = 0x00;

	gtestStackInstrmem.Data[0xFF00] = TXS;

	const uint32_t ExpectedCycles = 2;
	const struct CPU cpuCopy = gtestStackInstrcpu;

	uint32_t numCycles = Execute(&gtestStackInstrcpu, &gtestStackInstrmem, ExpectedCycles);

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestStackInstrcpu.sp, 0xFF);
	CheckStatusFlag(gtestStackInstrcpu, cpuCopy);
}

TEST(testJumpInstructions, PHA_TEST)				// PHA
{
	ResetCpu(&gtestStackInstrcpu, &gtestStackInstrmem);
	gtestStackInstrcpu.pc = 0xFF00;

	gtestStackInstrcpu.acc = 0x42;

	gtestStackInstrmem.Data[0xFF00] = PHA;

	const uint32_t ExpectedCycles = 3;
	const struct CPU cpuCopy = gtestStackInstrcpu;

	uint32_t numCycles = Execute(&gtestStackInstrcpu, &gtestStackInstrmem, ExpectedCycles);

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestStackInstrcpu.acc, gtestStackInstrmem.Data[SPtoWord(&gtestStackInstrcpu) + 1]);
	CheckStatusFlag(gtestStackInstrcpu, cpuCopy);
}

TEST(testJumpInstructions, PHP_TEST)				// PHP
{
	ResetCpu(&gtestStackInstrcpu, &gtestStackInstrmem);
	gtestStackInstrcpu.pc = 0xFF00;

	*gtestStackInstrcpu.Flags = 0xCC;

	gtestStackInstrmem.Data[0xFF00] = PHP;

	const uint32_t ExpectedCycles = 3;
	const struct CPU cpuCopy = gtestStackInstrcpu;

	uint32_t numCycles = Execute(&gtestStackInstrcpu, &gtestStackInstrmem, ExpectedCycles);

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(0xCC, gtestStackInstrmem.Data[SPtoWord(&gtestStackInstrcpu) + 1]);
	CheckStatusFlag(gtestStackInstrcpu, cpuCopy);
}

TEST(testJumpInstructions, PLA_TEST)				// PLA
{
	ResetCpu(&gtestStackInstrcpu, &gtestStackInstrmem);
	gtestStackInstrcpu.pc = 0xFF00;

	gtestStackInstrcpu.acc = 0x00;
	gtestStackInstrmem.Data[SPtoWord(&gtestStackInstrcpu) + 1] = 0x42;

	gtestStackInstrmem.Data[0xFF00] = PLA;

	const uint32_t ExpectedCycles = 4;
	const struct CPU cpuCopy = gtestStackInstrcpu;

	uint32_t numCycles = Execute(&gtestStackInstrcpu, &gtestStackInstrmem, ExpectedCycles);

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestStackInstrcpu.acc, gtestStackInstrmem.Data[SPtoWord(&gtestStackInstrcpu) + 1]);
	CheckStatusFlag(gtestStackInstrcpu, cpuCopy);
}


TEST(testJumpInstructions, PLP_TEST)				// PLP
{
	ResetCpu(&gtestStackInstrcpu, &gtestStackInstrmem);
	gtestStackInstrcpu.pc = 0xFF00;

	*gtestStackInstrcpu.Flags = 0x00;
	gtestStackInstrmem.Data[SPtoWord(&gtestStackInstrcpu) + 1] = 0x42;

	gtestStackInstrmem.Data[0xFF00] = PLP;

	const uint32_t ExpectedCycles = 4;
	const struct CPU cpuCopy = gtestStackInstrcpu;

	uint32_t numCycles = Execute(&gtestStackInstrcpu, &gtestStackInstrmem, ExpectedCycles);

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(*gtestStackInstrcpu.Flags, gtestStackInstrmem.Data[SPtoWord(&gtestStackInstrcpu) + 1]);
	CheckStatusFlag(gtestStackInstrcpu, cpuCopy);
}