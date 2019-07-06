#ifndef _TIMER_H_
#define _TIMER_H_

#include "mcu.h"

class Timer
{
    public:
        Timer(MCU* _mcu);
        ~Timer();

        union // Divider
        {
            uint16 div;
            struct { uint8 div_low, div_high; };
        };

        uint8 tma;
        uint8 tima;

        enum Speed : uint8
        {
            freq_262KHz = 0x01,
            freq_65KHz  = 0x02,
            freq_16KHz  = 0x03,
            freq_4KHz   = 0x00
        }

        bool timer_interrupt_enabled;

    private:

        MCU* mcu;
};

#endif // _TIMER_H_