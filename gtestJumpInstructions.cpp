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
	gtestJumpInstrmem.Data[0x8000] = RTS;
	gtestJumpInstrmem.Data[0xFF03] = LDA_IM;
	gtestJumpInstrmem.Data[0xFF04] = 0x42;

	const uint32_t ExpectedCycles = 6 + 6 + 2;

	uint32_t numCycles = Execute(&gtestJumpInstrcpu, &gtestJumpInstrmem, ExpectedCycles);
	const struct CPU cpuCopy = gtestJumpInstrcpu;

	EXPECT_EQ(numCycles, ExpectedCycles);
	EXPECT_EQ(gtestJumpInstrcpu.acc, 0x42);
	EXPECT_EQ(gtestJumpInstrcpu.sp, cpuCopy.sp);
}