#include "unity.h"
#include "propeller.h"

#include "c++-alloc.h"

#include "pwm32.h"
#include "pulsewidthreader/pulse_width_reader.h"

#include "board/board_unit_tester.h"



PWM32 * sut;

PulseWidthReader * pwr;




const int outputPin = Board::kPinTie1a;
const int inputPinMask = 1 << Board::kPinTie1b;


int MicrosecondsToClockCycles(const int microseconds){
    return (CLKFREQ*microseconds)/1000000;
}

int ClockCyclesToMicroseconds(const int clockCycles){
    return clockCycles / (CLKFREQ/1000000);
}

void setUp(void){
    sut = new PWM32();
    sut->Start();
    
    pwr = new PulseWidthReader();
    pwr->Start(inputPinMask);
    
    waitcnt(CLKFREQ/10 + CNT);
    
}


void tearDown(void){
    sut->Stop();
    delete sut;
    sut = NULL;
    
    pwr->Stop();
    delete pwr;
    pwr = NULL;
    
}



void helper_CheckWidths(const int highMicroseconds, const int lowMicroseconds){
    TEST_ASSERT_INT_WITHIN(PWM32::Resolution*2/1000, highMicroseconds, 
            ClockCyclesToMicroseconds(pwr->getHighTime(0)));
    
    TEST_ASSERT_INT_WITHIN(PWM32::Resolution*2/1000, lowMicroseconds, 
            ClockCyclesToMicroseconds(pwr->getLowTime(0)));
}


void helper_TestDuty(const int percent, const int period){
    sut->Duty(outputPin, percent, period);
    waitcnt(CLKFREQ/10 + CNT);
    
   //while(true){}
    
    helper_CheckWidths((period * percent) / 100, (period * (100 - percent)) / 100);
}


void helper_TestServo(const int pulsewidth){
    sut->Servo(outputPin, pulsewidth);
    waitcnt(CLKFREQ/10 + CNT);
    
    //while(true){}
    
    helper_CheckWidths(pulsewidth, 20000-pulsewidth);
}

void helper_TestPWM(const int high, const int low){
    sut->PWM(outputPin, high, low);
    waitcnt(CLKFREQ/10 + CNT);
    
    //while(true){}
    
    helper_CheckWidths(high, low);
}




void test_50PercentDuty(void){
    helper_TestDuty(50, 1000);
}

void test_20PercentDuty(void){
    helper_TestDuty(20, 1000);
}

void test_80PercentDuty(void){
    helper_TestDuty(80, 1000);
}



void test_ServoMiddle(void){
    helper_TestServo(1500);
}

void test_ServoLow(void){
    helper_TestServo(1000);
}

void test_ServoHigh(void){
    helper_TestServo(2000);
}

void test_ServoVeryLow(void){
    helper_TestServo(500);
}

void test_ServoVeryHigh(void){
    helper_TestServo(2500);
}

void test_pwmEven(void){
    helper_TestPWM(1000, 1000);
}

void test_pwmLongHigh(void){
    helper_TestPWM(2000, 100);
}

void test_pwmLongLow(void){
    helper_TestPWM(300, 1800);
}

void test_pwmPrimeTimes(void){
   helper_TestPWM(773, 907);
    
}





