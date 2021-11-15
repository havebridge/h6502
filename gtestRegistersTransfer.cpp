#include "gtest/gtest.h"
#include "6502.h"


struct CPU gtestTcpu;
struct memory gtestTmem;


static void CheckStatusFlag(const struct CPU& cpu1, const struct CPU& cpu2)
{
	EXPECT_FALSE(cpu1.Flags[zeroFlag]);
	EXPECT_FALSE(cpu1.Flags[negativeFlag - 1]);
	EXPECT_EQ(cpu1.Flags[interruptDisable], cpu2.Flags[interruptDisable]);
	EXPECT_EQ(cpu1.Flags[decimalMode], cpu2.Flags[decimalMode]);
	EXPECT_EQ(cpu1.Flags[breakCommand], cpu2.Flags[breakCommand]);
	EXPECT_EQ(cpu1.Flags[overflowFlag], cpu2.Flags[overflowFlag]);
}

static void gtestTransferRegIM(struct CPU* cpu, struct memory* mem, byte const opcode, byte* source, byte* destination)
{
	ResetCpu(cpu, mem);

	mem->Data[0xFFFC] = opcode;

	uint32_t numCycles = Execute(cpu, mem, 2);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*source, *destination);
	EXPECT_EQ(numCycles, 2);
	//EXPECT_FALSE(cpu->Flags[zeroFlag]);
}

TEST(testTransferReg, TAX_IM_TEST)
{
	gtestTransferRegIM(&gtestTcpu, &gtestTmem, TAX_IM, &gtestTcpu.acc, &gtestTcpu.x);
}

TEST(testTransferReg, TAY_IM_TEST)
{
	gtestTransferRegIM(&gtestTcpu, &gtestTmem, TAY_IM, &gtestTcpu.acc, &gtestTcpu.y);
}

TEST(testTransferReg, TXA_IM_TEST)
{
	gtestTransferRegIM(&gtestTcpu, &gtestTmem, TXA_IM, &gtestTcpu.x, &gtestTcpu.acc);
}

TEST(testTransferReg, TXY_IM_TEST)
{
	gtestTransferRegIM(&gtestTcpu, &gtestTmem, TYA_IM, &gtestTcpu.y, &gtestTcpu.acc);
}