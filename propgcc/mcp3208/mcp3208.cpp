#include <propeller.h>
#include "mcp3208.h"

extern char _load_start_mcp3208_cog[];

void MCP3208::Start(const int dataPin, const int clockPin,
        const int selectPin, const int mode, const int Xpin, const int Ypin) {
    
    if (Xpin == -1 && Ypin == -1) {
        //No DACs
        Startx(dataPin, clockPin, selectPin, mode, 0);
    } else if (Xpin != -1 && Ypin == -1) {
        //One DAC
        Startx(dataPin, clockPin, selectPin, mode, (Xpin & 0x1f) | 0x80);
    } else {
        //Two DACs
        Startx(dataPin, clockPin, selectPin, mode,
                (((Ypin & 0x1f) | 0x80) << 8) + ((Xpin & 0x1f) | 0x80));
    }
}

void MCP3208::Startx(const int dataPin, const int clockPin,
        const int selectPin, const int mode, const int Dacmode) {

    volatile void * asm_reference = NULL;
    __asm__ volatile ("mov %[asm_reference], #MCP3208_Entry\n\t"
            : [asm_reference] "+r" (asm_reference));

    Stop();

    Ins[0] = dataPin;
    Ins[1] = clockPin;
    Ins[2] = selectPin;
    Ins[3] = mode;

    Count = Dacmode;

    Cog = cognew(_load_start_mcp3208_cog, Ins) + 1;
}

void MCP3208::Stop(void) {
    if (Cog) {
        cogstop(Cog - 1);
        Cog = 0;
    }
}

int MCP3208::In(const int channel) {

    // Get pull the 16 bit word out of the 32 byte word.
    int result = Ins[channel / 2];

    // If index is odd then pull the upper word, if even pull the lower word.
    result = channel & 0x1 ? result >> 16 : result;

    // Make sure that we get just what we are interested in.
    return result & 0xFFFF;
}

int MCP3208::Average(const int channel, const int sampleCount) {
    //TODO(SRLM): What is C and Count?
    int sampleTotal = 0;
    int C = Count;
    for (int i = 0; i < sampleCount; i++) {
        while (C == Count) {
        }
        sampleTotal += In(channel);
        C++;

    }

    return sampleTotal / sampleCount;
}

void MCP3208::Out(const short X, const short Y) {
    Dacx = X << 16;
    Dacy = Y << 16;
}

