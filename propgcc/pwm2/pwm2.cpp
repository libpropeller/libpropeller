#include <propeller.h>
#include "pwm2.h"

extern char _load_start_pwm2_cog[];

void PWM2::Start(void) {

    volatile void * asm_reference = NULL;
    __asm__ volatile ( "mov %[asm_reference], #PWM2_Entry \n\t"
            : [asm_reference] "+r" (asm_reference));

    SetDutyX(0);
    SetDutyY(0);
    SetPinX(-1);
    SetPinY(-1);

    SetFrequency(kDefaultFrequency);
    Cog = cognew(_load_start_pwm2_cog, &Periodxy) + 1;
}

void PWM2::Stop(void) {
    if (Cog > 0) {
        SetDutyX(0);
        SetDutyY(0);
        waitcnt(Periodxy * 2 + CNT);
        cogstop(Cog - 1);
        Cog = 0;
    }

}

void PWM2::SetPinX(const int Pinx) {
    pin_x = Pinx;
    if (pin_x == -1) {
        Pinxmask = 0;
        Ctrxval = 0;

    } else {
        Pinxmask = (1 << Pinx);
        Ctrxval = ((4 << 26) + Pinx);
    }
}

void PWM2::SetPinY(const int Piny) {
    pin_y = Piny;
    if (pin_y == -1) {
        Pinymask = 0;
        Ctryval = 0;
    } else {
        Pinymask = (1 << Piny);
        Ctryval = ((4 << 26) + Piny);
    }
}

void PWM2::SetDutyX(const int Percent) {
    Percentx = Percent;
    Dutyx = ((Percent * Periodxy) / 100);
}

void PWM2::SetDutyY(const int Percent) {
    Percenty = Percent;
    Dutyy = ((Percent * Periodxy) / 100);
}

void PWM2::SetFrequency(const int Basefrequency) {
    Periodxy = (CLKFREQ / Basefrequency);
    SetDutyX(Percentx);
    SetDutyY(Percenty);
}
