#ifndef libpropeller_mcp3208_h_
#define libpropeller_mcp3208_h_

/** MCP3208 8 channel ADC Class.
 * 
 * When the MCP3208 is configured for 3.3v operation each bit represents 0.00080566406v.
 * 
 * The MCP3208 requires three connections to the Propeller: clock, chip select,
 * and data (tie together Dout and Din).
 * 
 * This object optionally provides two DACs as well. To enable these you need to
 * have one or two Propeller I/O pins available. Each DAC I/O pin needs to have
 * a low pass RC filter. I reccommend R = 1k and C = 0.1uF. With this
 * configuration you should avoid changing the DAC more than once a millisecond
 * (the cut off frequency is 1591Hz).
 * 
 * This driver does continuous sampling in the background.
 * 
 * This object is based on MCP3208.spin v1.0 written by Chip Gracey. Thanks!
 * 
 * @author srlm (srlm@srlmproductions.com)
 */

class MCP3208 {
public:
    /** Start the ADC driver in a new cog.
     * 
     * Be sure to give the driver sufficient time to start up.
     * 
     * @param dataPin The data in and data out pins on the MCP3208
     * @param clockPin The clock pin on the MCP3208
     * @param selectPin The select pin on the MCP3208
     * @param Mode Channel enables in bits [0..7], differential mode enable in
     *                  bits [8..15]. Most applications should set this to 0xFF
     * @param dacAPin If desired, the pin number for a DAC (see discussion above)
     * @param dacBPin If desired, the pin number for a DAC (see discussion above)
     */
    void Start(const int dataPin, const int clockPin,
            const int selectPin, const int Mode = 0xFF,
            const int dacAPin = -1, const int dacBPin = -1);

    
    /** Stop the driver and free a cog.
     */
    void Stop(void);
    
    /** Read a channel input.
     * 
     * @param Channel The channel [0..7] to read
     * @return The ADC reading in the range [0..4096]
     */
    int In(const int Channel);
    
    /** Sample a channel n times and average the results.
     * 
     * This function is useful for reducing noisy readings.
     * 
     * @param Channel The channel [0..7] to read
     * @param N The number of samples to average
     */
    int Average(const int Channel, const int N);
    
    
    /** Output analog values (if enabled during Start)
     * 
     * The range is 0 (ground) to 65535 (3.3v) for the output.
     * 
     * @param aOutput The output value for channel a
     * @param bOutput The output value for channel b
     */
    void Out(const short aOutput, const short bOutput = -1);
private:
    int Cog;

    //Warning: do not rearrange these variables. The assembly relies on the order.
    int volatile Ins[4];
    int volatile Count;
    int volatile Dacx, Dacy;

    void Startx(const int dataPin, const int clockPin,
            const int selectPin, const int mode, const int Dacmode);
};

#endif // libpropeller_mcp3208_h_
