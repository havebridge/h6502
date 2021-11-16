#include "gtest/gtest.h"
#include "6502.h"

struct CPU gtestBranchcpu;
struct memory gtestBranchmem;


static void CheckStatusFlag(const struct CPU& cpu1, const struct CPU& cpu2)
{
	EXPECT_EQ(cpu1.Flags[zeroFlag], cpu2.Flags[zeroFlag]);
	EXPECT_EQ(cpu1.Flags[negativeFlag - 1], cpu2.Flags[negativeFlag - 1]);
	EXPECT_EQ(cpu1.Flags[interruptDisable], cpu2.Flags[interruptDisable]);
	EXPECT_EQ(cpu1.Flags[decimalMode], cpu2.Flags[decimalMode]);
	EXPECT_EQ(cpu1.Flags[breakCommand], cpu2.Flags[breakCommand]);
	EXPECT_EQ(cpu1.Flags[overflowFlag], cpu2.Flags[overflowFlag]);
}

TEST(testBranchRegister, BEQ_TEST)
{
	ResetCpu(&gtestBranchcpu, &gtestBranchmem);

	gtestBranchcpu.Flags[zeroFlag] = true;
	gtestBranchcpu.pc = 0xFF00;

	gtestBranchmem.Data[0xFF00] = BEQ;
	gtestBranchmem.Data[0xFF01] = 0x1;

	uint32_t numCycles = Execute(&gtestBranchcpu, &gtestBranchmem, 3);
	const struct CPU cpuCopy = gtestBranchcpu;

	EXPECT_EQ(numCycles, 3);
	EXPECT_EQ(gtestBranchcpu.pc, 0xFF03);


	CheckStatusFlag(gtestBranchcpu, cpuCopy);
}

TEST(testBranchRegister, BEQ_ZERO_FLAG_TEST)
{
	ResetCpu(&gtestBranchcpu, &gtestBranchmem);

	gtestBranchcpu.Flags[zeroFlag] = false;
	gtestBranchcpu.pc = 0xFF00;

	gtestBranchmem.Data[0xFF00] = BEQ;
	gtestBranchmem.Data[0xFF01] = 0x1;

	uint32_t numCycles = Execute(&gtestBranchcpu, &gtestBranchmem, 2);
	const struct CPU cpuCopy = gtestBranchcpu;

	EXPECT_EQ(numCycles, 2);
	EXPECT_EQ(gtestBranchcpu.pc, 0xFF02);


	CheckStatusFlag(gtestBranchcpu, cpuCopy);
}

TEST(testBranchRegister, BEQ_NEW_PAGE_TEST)
{
	ResetCpu(&gtestBranchcpu, &gtestBranchmem);

	gtestBranchcpu.Flags[zeroFlag] = true;
	gtestBranchcpu.pc = 0xFEFD;

	gtestBranchmem.Data[0xFEFD] = BEQ;
	gtestBranchmem.Data[0xFEFE] = 0x1;

	uint32_t numCycles = Execute(&gtestBranchcpu, &gtestBranchmem, 5);
	const struct CPU cpuCopy = gtestBranchcpu;

	EXPECT_EQ(numCycles, 5);
	EXPECT_EQ(gtestBranchcpu.pc, 0xFF00);


	CheckStatusFlag(gtestBranchcpu, cpuCopy);
}