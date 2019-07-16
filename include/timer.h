#ifndef _TIMER_H_
#define _TIMER_H_

#include "mcu.h"

class Timer
{
    public:
        Timer(MCU* _mcu);

        enum Speed : uint8
        {
            freq_262KHz = 0x01,
            freq_65KHz  = 0x02,
            freq_16KHz  = 0x03,
            freq_4KHz   = 0x00
        };

        void step(uint16 elapsed_cycles);

    private:

        MCU* mcu;

        uint16& div;
        uint8&  div_low;
        uint8&  div_high;

        uint8& tima;
        uint8& tma;
        uint8& tac;

        uint8& if_register;

        bool prev_edge = false;
};

#endif // _TIMER_H_
