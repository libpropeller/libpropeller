#include "vnh2sp30.h"

vnh2sp30::vnh2sp30() {
    pwm32 = NULL;
    pwm2 = NULL;
    channel = NONE;
}

void vnh2sp30::Init(PWM32 * new_pwm, const int new_pwmPin, const int inApin, const int inBpin) {
    inA = Pin(inApin);
    inB = Pin(inBpin);
    pwmPin = Pin(new_pwmPin);

    inA.low();
    inB.low();
    pwmPin.low();

    pwm32 = new_pwm;

    SetDirection(FORWARD);
    SetSpeed(0);

}

void vnh2sp30::Init(PWM2 * new_pwm, const PwmChannel new_channel,
        const int new_pwmPin, const int inApin, const int inBpin) {
    inA = Pin(inApin);
    inB = Pin(inBpin);
    pwmPin = Pin(new_pwmPin);

    inA.low();
    inB.low();
    pwmPin.low();


    pwm2->SetFrequency(kFrequency);

    pwm2 = new_pwm;
    channel = new_channel;

    if (channel == X) {
        pwm2->SetPinX(pwmPin.getPin());
    } else if (channel == Y) {
        pwm2->SetPinY(pwmPin.getPin());
    }

    SetDirection(FORWARD);
    SetSpeed(0);
}

vnh2sp30::~vnh2sp30() {
    SetSpeed(0);
    waitcnt(CLKFREQ/10 + CNT);
    inA.input();
    inB.input();
    pwmPin.input();
}

void vnh2sp30::SetDirection(const Direction direction) {
    if (direction == FORWARD) {
        inA.high();
        inB.low();
    } else { //REVERSE
        inA.low();
        inB.high();
    }
}

void vnh2sp30::Set(const Direction direction, const int speed) {
    SetDirection(direction);
    SetSpeed(speed);
}

void vnh2sp30::SetSpeed(const int speed){
    int adjustedSpeed;
    if(speed < 0){
        adjustedSpeed = 0;
    }else if(speed > 100){
        adjustedSpeed = 100;
    }else{
        adjustedSpeed = speed;
        
    }
    
    if(pwm32 != NULL){
        SetSpeedPwm32(adjustedSpeed);
    }else if (pwm2 != NULL){
        SetSpeedPwm2(adjustedSpeed);
    }
}


void vnh2sp30::SetSpeedPwm2(const int speed) {

    if (channel == X) {
        pwm2->SetDutyX(speed);
    } else if (channel == Y) {
        pwm2->SetDutyY(speed);
    }
}

void vnh2sp30::SetSpeedPwm32(const int speed){
    
    pwm32->Duty(pwmPin.getPin(), speed, 1000000/kFrequency);
}


