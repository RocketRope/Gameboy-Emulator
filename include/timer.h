#ifndef _TIMER_H_
#define _TIMER_H_

#include "memory.h"

class Timer
{
    public:
    
        Timer(MCU* _mcu);

        enum SPEED : uint8
        {
            FREQ_262kHz = 0x01,
            FREQ_65kHz  = 0x02,
            FREQ_16kHz  = 0x03,
            FREQ_4kHz   = 0x00
        };

        void step(uint16 elapsed_clocks);

    private:

        MCU* mcu;

        uint16& div;
        uint8&  div_low;
        uint8&  div_high;

        uint8& tima;
        uint8& tma;
        uint8& tac;

        Bit<2> if_timer_flag;

        bool prev_edge = false;
};

#endif // _TIMER_H_
