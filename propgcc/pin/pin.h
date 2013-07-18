/* Pin.h - Pin class to allow access to single pins
 * 
 * 
 * What's with the __attribute__((always_inline))?
 * I (SRLM) found that the forced inlining is essential to performance. Without
 * it GCC would not inline a function (even if marked) if it was used more than
 * two or three times. This has a rather drastic performance impact. It does
 * come at the expense of some space, however. For example, the high() function
 * expands to 5 instructions:
 *
 *     mov	r7, OUTA
 *     rdlong	r6, r3
 *     or	r7, r6
 *     mov	OUTA, r7
 *     or	DIRA,r6
 * 
 * If you use high() in many places the total could add up. On the other hand,
 * the forced inlining may give the optimizer more chances to make things small.
 * You should try it both ways and compare.
 * 
 * * Hardware:
 * To read 3.3v signals just connect the signal directly to any I/O pin. To read
 * a 5v signal connect it to any I/O pin through a 3.3kOhm (or higher) resistor.
 * To read a higher voltage, use the following equation to determine the proper
 * series voltage:
 * resistance = (voltage - 3.3v - 0.6v) / 500uA
 * 
 * 
 * 
 * 
 * Modified by SRLM.
 * 
 * Copyright (c) 2012 David Michael Betz
 * 
 */

// Comment out one definition of INLINE.
#define INLINE __attribute__((always_inline)) inline 
//#define INLINE inline

#include <propeller.h>

class Pin {
public:
    /** Create the pin instance.
     * 
     * @param pin the I/O pin number (0 through 31) to control.
     */
    Pin(int pin);

    /** Set pin to output high.
     */
    void high(void);

    /** Set pin to output low.
     */
    void low(void);

    /** Toggle the pin on output. If it was low, make it high. If it was high, 
     * make it low.
     */
    void toggle(void);

    /** Set the pin to input and get it's value.
     * 
     * @return 1 if the pin is high, 0 if it is low.
     */
    int input(void);

    /** Output a value on the pin.
     * 
     * @param high true to output high, false to output low.
     */
    void output(int setting);

    /** Get the input or output direction.
     * 
     * @warning Can only tell if the output bit is set for this cog (not all cogs)
     * 
     * @return true if output, false if input
     */
    bool isOutput(void);

private:
    unsigned int pin_mask;
    void output();
};

INLINE Pin::Pin(int pin) : pin_mask(1 << pin) {
}

INLINE void Pin::high() {
    OUTA |= pin_mask;
    output();
}

INLINE void Pin::low() {
    OUTA &= ~pin_mask;
    output();
}

INLINE void Pin::toggle() {
    OUTA ^= pin_mask;
    output();
}

INLINE void Pin::output(int setting) {
    if (setting == 1)
        high();
    else
        low();
}

INLINE bool Pin::isOutput() {
    return (DIRA & pin_mask) ? true : false;
}

INLINE int Pin::input() {
    DIRA &= ~pin_mask;
    return (INA & pin_mask) != 0;
}

INLINE void Pin::output() {
    DIRA |= pin_mask;
}
