#include "gtest/gtest.h"
#include "6502.h"

struct CPU gtestJumpInstrcpu;
struct memory gtestJumpInstrmem;


TEST(testJumpInstructions, JSR_TEST)
{
	ResetCpu(&gtestJumpInstrcpu, &gtestJumpInstrmem);
	gtestJumpInstrcpu.pc = 0xFF00;

	gtestJumpInstrmem.Data[0xFF00] = JSR;
	gtestJumpInstrmem.Data[0xFF01] = 0x00;
	gtestJumpInstrmem.Data[0xFF02] = 0x80;
	gtestJumpInstrmem.Data[0x8000] = RTS;;

	const uint32_t ExpectedCycles = 6 + 6;

	uint32_t numCycles = Execute(&gtestJumpInstrcpu, &gtestJumpInstrmem, ExpectedCycles);
	const struct CPU cpuCopy = gtestJumpInstrcpu;

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestJumpInstrcpu.sp, cpuCopy.sp);
	EXPECT_EQ(gtestJumpInstrcpu.pc, 0xFF03);
}

TEST(testJumpInstructions, JUMP_ABS_TEST)
{
	ResetCpu(&gtestJumpInstrcpu, &gtestJumpInstrmem);
	gtestJumpInstrcpu.pc = 0xFF00;

	gtestJumpInstrmem.Data[0xFF00] = JMP_ABS;
	gtestJumpInstrmem.Data[0xFF01] = 0x00;
	gtestJumpInstrmem.Data[0xFF02] = 0x80;


	const uint32_t ExpectedCycles = 3;

	uint32_t numCycles = Execute(&gtestJumpInstrcpu, &gtestJumpInstrmem, ExpectedCycles);
	const struct CPU cpuCopy = gtestJumpInstrcpu;

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestJumpInstrcpu.sp, cpuCopy.sp);
	EXPECT_EQ(gtestJumpInstrcpu.pc, 0x8000);
}


TEST(testJumpInstructions, JUMP_IND_TEST)
{
	ResetCpu(&gtestJumpInstrcpu, &gtestJumpInstrmem);
	gtestJumpInstrcpu.pc = 0xFF00;

	gtestJumpInstrmem.Data[0xFF00] = JMP_IND;
	gtestJumpInstrmem.Data[0xFF01] = 0x00;
	gtestJumpInstrmem.Data[0xFF02] = 0x80;
	gtestJumpInstrmem.Data[0x8000] = 0x00;
	gtestJumpInstrmem.Data[0x8001] = 0x90;

	const uint32_t ExpectedCycles = 5;

	uint32_t numCycles = Execute(&gtestJumpInstrcpu, &gtestJumpInstrmem, ExpectedCycles);
	const struct CPU cpuCopy = gtestJumpInstrcpu;

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestJumpInstrcpu.sp, cpuCopy.sp);
	EXPECT_EQ(gtestJumpInstrcpu.pc, 0x9000);
}