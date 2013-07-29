#ifndef libpropeller_pwm2_h__
#define libpropeller_pwm2_h__


/** Run a high speed pulse width modulation (PWM) class that can output up to
 * two channels of duty cycled PWM at a single frequency.
 * 
 * The two channels are channel X and channel Y.
 * 
 * Requires a cog to operate. No external hardware is required beyond whatever
 * you are PWM'ing.
 * 
 * You can use this class to PWM H bridges, LEDs, audio, etc.
 * 
 * @todo(SRLM): figure out the maximum frequency (as a function of clock speed)
 * 
 * 
 */
class PWM2 {
public:
    
    /** Start a PWM driver in a new cog.
     * 
     */
    void Start(void);
    
    /** Stop the PWM driver (if running)
     * 
     */
    void Stop(void);
    
    /** Set a pin to PWM out on.
     * 
     * You can change this during operation.
     * 
     * @param pinX The I/O pin [0..31] to set the X channel on.
     */
    void SetPinX(const int pinX);
    
    /** Set a pin to PWM out on.
     * 
     * You can change this during operation.
     * 
     * @param pinY The I/O pin [0..31] to set the Y channel on.
     */
    void SetPinY(const int pinY);
    
    /** Output a square wave with specified duty cycle.
     * 
     * @param percent The duty [0..100] to PWM. 0 is off, 100 is full on.
     */
    void SetDutyX(const int percent);
    
    /** Output a square wave with specified duty cycle.
     * 
     * You can change this during operation.
     * 
     * @param percent The duty [0..100] to PWM. 0 is off, 100 is full on.
     */
    void SetDutyY(const int percent);
    
    
    /** Set the frequency to output channel X and Y on.
     * 
     * @todo(SRLM): SetFrequency doesn't seem to actually change the frequency (at least once the cog is started...)
     * 
     * @param frequency The frequency in Hz, [0..40000??]
     */
    void SetFrequency(const int frequency);
private:
    int Cog;
    int Percentx, Percenty;
    int pin_x, pin_y;
    int volatile Periodxy;
    int volatile Dutyx, Pinxmask, Ctrxval;
    int volatile Dutyy, Pinymask, Ctryval;

    static const int kDefaultFrequency = 12000;
};

#endif // libpropeller_pwm2_h__