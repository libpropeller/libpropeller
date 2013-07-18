#ifndef libpropeller_full_duplex_serial_h_
#define libpropeller_full_duplex_serial_h_


//Warning: To change the buffer size you must change it here and in the .S file
#define buffersize 256
#define buffermask (buffersize - 1)

/** This driver, once started, implements a serial port in one cog.
 * 
 * Hardware:
 * To read 3.3v signals just connect the signal directly to any I/O pin. To read
 * a 5v signal connect it to any I/O pin through a 3.3kOhm (or higher) resistor.
 * 
 * Software:
 * The maximum standard baud rate on an 80MHz clock is 230400 baud.
 * 
 * 
 * 
 * This object is derived from FullDuplexSerial.spin v1.2.1 by Chip Gracey, Jeff
 * Martin, and Daniel Harris. Thanks!
 * 
 * @author SRLM(srlm@srlrmproductions.com)
 * 
 */

class FullDuplexSerial {
public:
    /** Start serial driver in new cog.
     * 
     * @param Rxpin Input to the Propeller
     * @param Txpin Output from the Propeller
     * @param Mode  Bitwise mode configuration variable:
     * - mode bit 0 = invert rx
     * - mode bit 1 = invert tx
     * - mode bit 2 = open-drain/source tx
     * - mode bit 3 = ignore tx echo on rx
     * @param Baudrate Any rate will work, up to the maximum baud
     */
    void Start(int Rxpin, int Txpin, int Mode, int Baudrate);

    /** Stop the serial driver (if running) and free a cog.
     */
    void Stop(void);

    /** Flush the receive buffer.
     * 
     */
    void GetFlush(void);

    /** Receive a byte of data.
     * 
     * If there are bytes in the buffer this function returns immediately.
     * Otherwise, it waits as specified by the timeout parameter.
     * 
     * @param timeout The maximum duration to wait for a byte in milliseconds.
     *                  A timeout of -1 indicates no wait.
     * @return -1 if a timeout, otherwise the byte received.
     */
    int Get(const int timeout = -1);


    /** Transmit a byte of data.
     * 
     * @param Txbyte the byte to send.
     */
    void Put(const char Txbyte);

    /** Transmit a C string.
     * 
     * Note: the null at the end is not transmitted.
     * 
     * @param string The null terminated string to transmit.
     */
    void Put(const char * string);

private:

    //The Dec, Hex, and Bin functions are left out in the hopes of getting a separate object to do the conversion.
    int PutDec(int Value);
    void PutHex(int Value, int Digits);
    void PutBin(int Value, int Digits);

    int Cog;

    //Warning: the order of these variables is important! Do not change.
    volatile int Rx_head;
    volatile int Rx_tail;
    volatile int Tx_head;
    volatile int Tx_tail;
    volatile int Rx_pin;
    volatile int Tx_pin;
    volatile int Rxtx_mode;
    volatile int Bit_ticks;
    volatile char * Buffer_ptr;
    volatile char Rx_buffer[buffersize];
    volatile char Tx_buffer[buffersize];

    int CheckBuffer(void);
};

#endif // libpropeller_full_duplex_serial_h_
