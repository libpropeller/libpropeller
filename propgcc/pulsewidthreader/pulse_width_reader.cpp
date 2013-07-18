#include <propeller.h>
#include "pulse_width_reader.h"

extern char _load_start_pulsewidthreader_cog[];

PulseWidthReader::PulseWidthReader() {
    Cog = 0;
}

void PulseWidthReader::Start(unsigned int Inputmask) {

    volatile void * asm_reference = NULL;
    __asm__ volatile ("mov %[asm_reference], #PulseWidthReader_Entry \n\t"
            : [asm_reference] "+r" (asm_reference));
    Stop();

    pinTimes[0] = Inputmask;
    Cog = cognew(_load_start_pulsewidthreader_cog, pinTimes) + 1;
}

void PulseWidthReader::Stop(void) {
    if (Cog != 0) {
        cogstop(Cog - 1);
        Cog = 0;
    }
}

int PulseWidthReader::getHighTime(int index) {
    return pinTimes[index * 2];
}

int PulseWidthReader::getLowTime(int index) {
    return pinTimes[index * 2 + 1];
}

