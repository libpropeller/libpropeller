
#ifndef libpropeller_pwm32_h__
#define libpropeller_pwm32_h__

#include <stdint.h>


/** Run up to 32 channels of low speed PWM (up to ~10kHz@80MHz clock).
 * 
 * No external hardware beyond the PWM devices is needed. Works with H bridges,
 * LEDs, servos, and anything else that takes a repeating PWM signal.
 * 
 */
class PWM32 {
public:
    static const int Resolution = 8200; //8.2 uS @80MHz

    static volatile uint8_t dat[];
    PWM32();


    /** Start a cog with the PWM runner.
     */
    void Start(void);


    /** Stop the cog, if running.
     */
    void Stop(void);



    /** Output a PWM wave with a calculated duty.
     * 
     * @param pin        The I/O pin to use (0..31)
     * @param duty_cycle The duty cycle (0..100) to use. 0 is off, 100 is full on.
     * @param period     The period (in uS) of the cycle.
     */
    void Duty(int pin, int duty_cycle, int period);


    /** Output a servo compatible pulse with a period of 20ms.
     * 
     * @param pin         The I/O pin to use (0..31)
     * @param pulse_width The width of the servo pulse in uS. Can be any value,
     *                          but most servos require a range (1000..2000)
     */
    void Servo(int pin, int pulse_width);

    /** Output a series of square waves with a high time of on_time
     * microseconds, and a period of (on_time + low_time) microseconds.
     * 
     * @param pin       The I/O pin to use (0..31)
     * @param on_time   The duration of the high time, in uS.
     * @param off_time  The duration of the low time, in uS.
     */
    void PWM(int pin, int on_time, int off_time);

private:

    static const int Updatetontoff = 1;
    static const int Io_state = 2;
    static const int Dutyoverride = 3;
    static const int Syncphase = 4;
    static const int Duty_default = 0;
    static const int Duty_100 = 1;
    static const int Duty_0 = 2;
    static const int Disablepin = 0;
    static const int Enablepin = 1;

    int cog;

    //These four variables must remain in the same order.
    volatile int command, argument_0, argument_1, argument_2;

    void Statemode(int Pin, int State);
    void Dutymode(int Pin, int Mode);


    // Phasesync is untested...
    void Phasesync(int Pin1, int Pin2, int Phase);

    /**
     * 
     * @param Pin
     * @param Ontime in units of resolution (decimal, 8.2)
     * @param Offtime
     * @return 
     */
    void InternalPWM(int Pin, int Ontime, int Offtime);
};

#endif // libpropeller_pwm32_h__
