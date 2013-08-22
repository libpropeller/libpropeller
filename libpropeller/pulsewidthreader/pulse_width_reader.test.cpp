#include "unity.h"
#include "pulse_width_reader.h"
#include "c++-alloc.h"

#include "board/board_unit_tester.h"
#include "pin/pin.h"


//TODO(SRLM): Add tests for startup conditions: reading right after start, reading after a single transition
//TODO(SRLM): Add test for reading a pin that has never changed while other pins have.

PulseWidthReader * sut;

const int kPulseMask = (1 << Board::kPinSqw)
| (1 << Board::kPinTie1b)
| (1 << Board::kPinTie2b);


Pin sqwOutputPin1(Board::kPinTie1a);
Pin sqwOutputPin2(Board::kPinTie2a);

const int stacksize = sizeof (_thread_state_t) + sizeof (int) * 30;
int stack [stacksize];


const int kSqwIndex = 0;
const int k1Index = 1;
const int k2Index = 2;

const int kDelta = 700;

void setUp(void) {
    sqwOutputPin1.input();
    sqwOutputPin2.input();

    sut = new PulseWidthReader();
    sut->Start(kPulseMask);
    waitcnt(CLKFREQ / 100 + CNT);
}

void tearDown(void) {
    sut->Stop();
    delete sut;
    sut = NULL;

}


int highDuration;
int lowDuration;
volatile bool runCog = false;

void cog_RunSquareWaveOutput(void * ignored) {
    unsigned int nextCNT = CNT;
    while (runCog) {
        sqwOutputPin1.high();
        nextCNT += highDuration;
        waitcnt(nextCNT);


        sqwOutputPin1.low();
        nextCNT += lowDuration;
        waitcnt(nextCNT);
    }

    cogstop(cogid());
}

void run_SquareWaveTest(const int highCycles, const int lowCycles) {
    runCog = true;
    highDuration = highCycles;
    lowDuration = lowCycles;

    cogstart(cog_RunSquareWaveOutput, NULL, stack, stacksize);
    waitcnt(CLKFREQ / 10 + CNT);

    TEST_ASSERT_INT_WITHIN(kDelta, highDuration, sut->getHighTime(k1Index));
    TEST_ASSERT_INT_WITHIN(kDelta, lowDuration, sut->getLowTime(k1Index));

    runCog = false;
    waitcnt(CLKFREQ / 10 + CNT);
}

//------------------------------------------------------------------------------

void test_EvenSquareWave(void) {
    run_SquareWaveTest(CLKFREQ / 1000, CLKFREQ / 1000);
}

void test_MostlyHighSquareWave(void) {
    run_SquareWaveTest(CLKFREQ / 100, CLKFREQ / 1000);
}

void test_MostlyLowSquareWave(void) {
    run_SquareWaveTest(CLKFREQ / 1000, CLKFREQ / 100);
}

void test_Something(void) {
    run_SquareWaveTest(CLKFREQ / 393, CLKFREQ / 484);
}


//------------------------------------------------------------------------------

void test_SingleHighPulse(void) {
    const int cycles = CLKFREQ / 1000;

    sqwOutputPin1.low();
    waitcnt(CLKFREQ / 100 + CNT);
    sqwOutputPin1.high();
    waitcnt(cycles + CNT);
    sqwOutputPin1.low();
    waitcnt(CLKFREQ / 100 + CNT);

    TEST_ASSERT_INT_WITHIN(kDelta, cycles, sut->getHighTime(k1Index));
}

void test_SingleLowPulse(void) {
    const int cycles = CLKFREQ / 1000;

    sqwOutputPin1.high();
    waitcnt(CLKFREQ / 100 + CNT);
    sqwOutputPin1.low();
    waitcnt(cycles + CNT);
    sqwOutputPin1.high();
    waitcnt(CLKFREQ / 100 + CNT);

    TEST_ASSERT_INT_WITHIN(kDelta, cycles, sut->getLowTime(k1Index));
}

//------------------------------------------------------------------------------

void test_32768SquareWave(void) {
    const int kClockFrequency = 32768;
    TEST_ASSERT_INT_WITHIN(130, CLKFREQ / (2 * kClockFrequency), sut->getHighTime(kSqwIndex));
    TEST_ASSERT_INT_WITHIN(130, CLKFREQ / (2 * kClockFrequency), sut->getLowTime(kSqwIndex));
}





