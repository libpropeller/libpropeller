#include <propeller.h>
#include "full_duplex_serial.h"

static inline int Rotl__(unsigned int a, unsigned int b) {
    return (a << b) | (a >> (32 - b));
}

static inline int Lookup__(int x, int b, int a[], int n) {
    int i = (x)-(b);
    return ((unsigned) i >= n) ? 0 : (a)[i];
}

extern char _load_start_full_duplex_serial_cog[];

void FullDuplexSerial::Start(int Rxpin, int Txpin, int Mode, int Baudrate) {

    volatile void * asm_reference = NULL;
    __asm__ volatile ( "mov %[asm_reference], #FullDuplexSerial_Entry \n\t"
            : [asm_reference] "+r" (asm_reference));


    Stop();

    Rx_head = 0;
    Rx_tail = 0;
    Tx_head = 0;
    Tx_tail = 0;

    Rx_pin = Rxpin;
    Tx_pin = Txpin;
    Rxtx_mode = Mode;

    Bit_ticks = (CLKFREQ / Baudrate);
    Buffer_ptr = Rx_buffer;

    Cog = cognew(_load_start_full_duplex_serial_cog, &Rx_head) + 1;

}

void FullDuplexSerial::Stop(void) {
    if (Cog > 0) {
        cogstop(Cog - 1);
        Cog = 0;
    }

    Rx_head = 0;
    Rx_tail = 0;
    Tx_head = 0;
    Tx_tail = 0;
    Rx_pin = 0;
    Tx_pin = 0;
    Rxtx_mode = 0;
    Bit_ticks = 0;
    Buffer_ptr = 0;
}

void FullDuplexSerial::GetFlush(void) {
    while (CheckBuffer() >= 0) {
    }
}

int FullDuplexSerial::CheckBuffer(void) {
    int Rxbyte = -1;
    if (Rx_tail != Rx_head) {
        Rxbyte = Rx_buffer[Rx_tail];
        Rx_tail = (Rx_tail + 1) & buffermask;
    }
    return Rxbyte;
}

int FullDuplexSerial::Get(const int timeout) {

    int Rxbyte = 0;
    if (timeout < 0) {
        while ((Rxbyte = CheckBuffer()) < 0) {
        }
    } else {
        int T = CNT;
        while (!(((Rxbyte = CheckBuffer()) >= 0)
                || (((CNT - T) / (CLKFREQ / 1000)) > timeout))) {
        }
    }
    return Rxbyte;

}

void FullDuplexSerial::Put(const char Txbyte) {
    while (!(Tx_tail != ((Tx_head + 1) & buffermask))) {
    }
    Tx_buffer[Tx_head] = Txbyte;
    Tx_head = ((Tx_head + 1) & buffermask);
    if (Rxtx_mode & 0x8) {
        Get();
    }
}

void FullDuplexSerial::Put(const char * Stringptr) {
    const int length = strlen(Stringptr);
    for (int i = 0; i < length; i++) {
        Put(Stringptr[i]);
    }

}

int FullDuplexSerial::PutDec(int Value) {
    int I;
    int result = 0;
    if (Value == (int) 0x80000000U) {
        //TODO(SRLM): take care of the MAX negative value (it can't be made positive...)
    }
    if (Value < 0) {
        Value = -Value;
        Put('-');
    }
    I = 1000000000;
    {
        int _idx__0043;
        for (_idx__0043 = 1; _idx__0043 <= 10; (_idx__0043 = (_idx__0043 + 1))) {
            if (Value >= I) {
                Put((((Value / I) + '0')));
                Value = (Value % I);
                result = -1;
            } else {
                if ((result) || (I == 1)) {
                    Put('0');
                }
            }
            I = (I / 10);
        }
    }
    return result;
}

void FullDuplexSerial::PutHex(int Value, int Digits) {
    static int look__0044[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70,};

    Value = (Value << ((8 - Digits) << 2));
    {
        int _idx__0045;
        int _limit__0046 = Digits;
        for (_idx__0045 = 1; _idx__0045 <= _limit__0046; (_idx__0045 = (_idx__0045 + 1))) {
            Put(Lookup__(((Value = (Rotl__(Value, 4))) & 0xf), 0, look__0044, 16));
        }
    }

}

void FullDuplexSerial::PutBin(int Value, int Digits) {
    Value = (Value << (32 - Digits));
    {
        int _idx__0047;
        int _limit__0048 = Digits;
        for (_idx__0047 = 1; _idx__0047 <= _limit__0048; (_idx__0047 = (_idx__0047 + 1))) {
            Put((((Value = (Rotl__(Value, 1))) & 0x1) + '0'));
        }
    }

}

