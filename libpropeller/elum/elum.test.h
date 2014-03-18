#include "unity.h"
#include "elum.h"
#include "c++-alloc.h"
//#include <stdio.h>

const int kPIN_LEDR = 5;
const int kPIN_MAX8819_EN123 = 6;
const int kPIN_LEDG = 7;
const int kPIN_BUTTON = 8;


libpropeller::Elum * sut;

class UnityTests {
public:

    static void setUp(void) {
        sut = new libpropeller::Elum(kPIN_LEDR, kPIN_LEDG, kPIN_BUTTON);
    }

    static void tearDown(void) {
        delete sut;
    }

    //static void test_Pattern(void){
    //	sut->Pattern(Elum::kJitterFast);
    //	
    //	
    //}

    static void test_LEDs(void) {
        sut->On(libpropeller::Elum::RED);
        //	waitcnt(CLKFREQ*2 + CNT);
        sut->Fade(10);
        //	waitcnt(CLKFREQ*2 + CNT);
        sut->Pattern(libpropeller::Elum::kManyFast);
        //	waitcnt(CLKFREQ*2 + CNT);
        TEST_IGNORE_MESSAGE("Must check results manually.");
    }

};
