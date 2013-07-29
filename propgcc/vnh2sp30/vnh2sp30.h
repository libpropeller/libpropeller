
#ifndef libpropeller_vnh2sp30_h__
#define libpropeller_vnh2sp30_h__

#include "pwm32.h"
#include "pwm2.h"

#include "pin.h"


class vnh2sp30 {
public:
    
    vnh2sp30();
    ~vnh2sp30();
    
    static const int kFrequency = 1000;
    //static const int kPeriod = CLKFREQ/kFrequency;
    //static const int kMinimumOffTime = CLKFREQ*7/1000000;

    
    enum Direction {FORWARD, REVERSE};
    enum PwmChannel {NONE, X, Y};
    
    
    void Init(PWM32 * new_pwm, const int new_pwmPin, const int inApin, const int inBpin);
    void Init(PWM2 * new_pwm, const PwmChannel channel, const int new_pwmPin, const int inApin, const int inBpin);
    
    void Set(const Direction direction, const int speed);
    void SetSpeed(const int speed);
    void SetDirection(const Direction direction);
    
private:
    
    Pin inA, inB, pwmPin;
    PWM32* pwm32;
    PWM2* pwm2;
    
    PwmChannel channel;
    
    
    static const int kResolution = 10;
    static const int kScale = 2147483647/(1 << (kResolution -1));
    
    void SetSpeedPwm2(const int speed);
    void SetSpeedPwm32(const int speed);
  
    
    
    
    
    
    
    
    
    
    
    
};

#endif // libpropeller_vnh2sp30_h__
