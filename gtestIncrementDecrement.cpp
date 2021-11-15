#include "gtest/gtest.h"
#include "6502.h"


struct CPU gtestDIcpu;
struct memory gtestDImem;

static void CheckStatusFlag(const struct CPU& cpu1, const struct CPU& cpu2)
{
	EXPECT_FALSE(cpu1.Flags[zeroFlag]);
	EXPECT_FALSE(cpu1.Flags[negativeFlag - 1]);
	EXPECT_EQ(cpu1.Flags[interruptDisable], cpu2.Flags[interruptDisable]);
	EXPECT_EQ(cpu1.Flags[decimalMode], cpu2.Flags[decimalMode]);
	EXPECT_EQ(cpu1.Flags[breakCommand], cpu2.Flags[breakCommand]);
	EXPECT_EQ(cpu1.Flags[overflowFlag], cpu2.Flags[overflowFlag]);
}


TEST(testIDRegister, INX_IM_ZERO_TEST)
{
	ResetCpu(&gtestDIcpu, &gtestDImem);

	gtestDIcpu.pc = 0xFF00;
	gtestDIcpu.x = 0x0;
	gtestDIcpu.Flags[zeroFlag] = true;
	gtestDIcpu.Flags[negativeFlag - 1] = true;
	
	gtestDImem.Data[0xFF00] = INX_IM;

	uint32_t numCycles = Execute(&gtestDIcpu, &gtestDImem, 2);
	const struct CPU cpuCopy = gtestDIcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestDIcpu.x, 0x1);

	EXPECT_FALSE(gtestDIcpu.Flags[zeroFlag]);
	EXPECT_FALSE(gtestDIcpu.Flags[negativeFlag - 1]);

	CheckStatusFlag(gtestDIcpu, cpuCopy);
}

TEST(testIDRegister, INX_IM_TEST)
{
	ResetCpu(&gtestDIcpu, &gtestDImem);

	gtestDIcpu.pc = 0xFF00;
	gtestDIcpu.x = 0x25;
	gtestDIcpu.Flags[zeroFlag] = true;
	gtestDIcpu.Flags[negativeFlag - 1] = true;

	gtestDImem.Data[0xFF00] = INX_IM;

	uint32_t numCycles = Execute(&gtestDIcpu, &gtestDImem, 2);
	const struct CPU cpuCopy = gtestDIcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestDIcpu.x, 0x26);

	EXPECT_FALSE(gtestDIcpu.Flags[zeroFlag]);
	EXPECT_FALSE(gtestDIcpu.Flags[negativeFlag - 1]);

	CheckStatusFlag(gtestDIcpu, cpuCopy);
}


TEST(testIDRegister, INY_IM_ZERO_TEST)
{
	ResetCpu(&gtestDIcpu, &gtestDImem);

	gtestDIcpu.pc = 0xFF00;
	gtestDIcpu.x = 0x0;
	gtestDIcpu.Flags[zeroFlag] = true;
	gtestDIcpu.Flags[negativeFlag - 1] = true;

	gtestDImem.Data[0xFF00] = INY_IM;

	uint32_t numCycles = Execute(&gtestDIcpu, &gtestDImem, 2);
	const struct CPU cpuCopy = gtestDIcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestDIcpu.y, 0x1);

	EXPECT_FALSE(gtestDIcpu.Flags[zeroFlag]);
	EXPECT_FALSE(gtestDIcpu.Flags[negativeFlag - 1]);

	CheckStatusFlag(gtestDIcpu, cpuCopy);
}

TEST(testIDRegister, INY_IM_TEST)
{
	ResetCpu(&gtestDIcpu, &gtestDImem);

	gtestDIcpu.pc = 0xFF00;
	gtestDIcpu.y = 0x25;
	gtestDIcpu.Flags[zeroFlag] = true;
	gtestDIcpu.Flags[negativeFlag - 1] = true;

	gtestDImem.Data[0xFF00] = INY_IM;

	uint32_t numCycles = Execute(&gtestDIcpu, &gtestDImem, 2);
	const struct CPU cpuCopy = gtestDIcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestDIcpu.y, 0x26);

	EXPECT_FALSE(gtestDIcpu.Flags[zeroFlag]);
	EXPECT_FALSE(gtestDIcpu.Flags[negativeFlag - 1]);

	CheckStatusFlag(gtestDIcpu, cpuCopy);
}


TEST(testIDRegister, DEX_IM_ZERO_TEST)
{
	ResetCpu(&gtestDIcpu, &gtestDImem);

	gtestDIcpu.pc = 0xFF00;
	gtestDIcpu.x = 0x0;
	gtestDIcpu.Flags[negativeFlag - 1] = false;

	gtestDImem.Data[0xFF00] = DEX_IM;

	uint32_t numCycles = Execute(&gtestDIcpu, &gtestDImem, 2);
	const struct CPU cpuCopy = gtestDIcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestDIcpu.x, 0xFF);

	EXPECT_TRUE(gtestDIcpu.Flags[negativeFlag - 1]);
}

TEST(testIDRegister, DEX_IM_TEST)
{
	ResetCpu(&gtestDIcpu, &gtestDImem);

	gtestDIcpu.pc = 0xFF00;
	gtestDIcpu.x = 0x25;
	gtestDIcpu.Flags[zeroFlag] = true;
	gtestDIcpu.Flags[negativeFlag - 1] = true;

	gtestDImem.Data[0xFF00] = DEX_IM;

	uint32_t numCycles = Execute(&gtestDIcpu, &gtestDImem, 2);
	const struct CPU cpuCopy = gtestDIcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestDIcpu.x, 0x24);

	EXPECT_FALSE(gtestDIcpu.Flags[zeroFlag]);
	EXPECT_FALSE(gtestDIcpu.Flags[negativeFlag - 1]);

	CheckStatusFlag(gtestDIcpu, cpuCopy);
}

TEST(testIDRegister, DEY_IM_ZERO_TEST)
{
	ResetCpu(&gtestDIcpu, &gtestDImem);

	gtestDIcpu.pc = 0xFF00;
	gtestDIcpu.y = 0x0;
	gtestDIcpu.Flags[negativeFlag - 1] = false;

	gtestDImem.Data[0xFF00] = DEY_IM;

	uint32_t numCycles = Execute(&gtestDIcpu, &gtestDImem, 2);
	const struct CPU cpuCopy = gtestDIcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestDIcpu.y, 0xFF);

	EXPECT_TRUE(gtestDIcpu.Flags[negativeFlag - 1]);
}

TEST(testIDRegister, DEY_IM_TEST)
{
	ResetCpu(&gtestDIcpu, &gtestDImem);

	gtestDIcpu.pc = 0xFF00;
	gtestDIcpu.y = 0x25;
	gtestDIcpu.Flags[zeroFlag] = true;
	gtestDIcpu.Flags[negativeFlag - 1] = true;

	gtestDImem.Data[0xFF00] = DEY_IM;

	uint32_t numCycles = Execute(&gtestDIcpu, &gtestDImem, 2);
	const struct CPU cpuCopy = gtestDIcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestDIcpu.y, 0x24);

	EXPECT_FALSE(gtestDIcpu.Flags[zeroFlag]);
	EXPECT_FALSE(gtestDIcpu.Flags[negativeFlag - 1]);

	CheckStatusFlag(gtestDIcpu, cpuCopy);
}