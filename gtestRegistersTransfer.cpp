#include "gtest/gtest.h"
#include "6502.h"


struct CPU gtestTcpu;
struct memory gtestTmem;


static void gtestTransferRegIM(struct CPU* cpu, struct memory* mem, byte const opcode, byte* source, byte* destination)
{
	ResetCpu(cpu, mem);

	mem->Data[0xFFFC] = opcode;

	uint32_t numCycles = Execute(cpu, mem, 2);
	const struct CPU cpuCopy = *cpu;

	EXPECT_EQ(*source, *destination);
	EXPECT_EQ(numCycles, 2);
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