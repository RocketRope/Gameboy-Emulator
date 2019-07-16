#include "timer.h"

Timer::Timer(MCU* _mcu) :
    mcu(_mcu),
    div( *((uint16*) (&(mcu->get_memory_reference(MCU::Addr::div - 1)))) ),
    div_low( mcu->get_memory_reference(0xFF03) ),
    div_high( mcu->get_memory_reference(MCU::Addr::div) ),
    tima( mcu->get_memory_reference(MCU::Addr::tima) ),
    tma( mcu->get_memory_reference(MCU::Addr::tma) ),
    tac( mcu->get_memory_reference(MCU::Addr::tac) ),
    if_register(mcu->get_memory_reference(MCU::Addr::if_))
{

}

void Timer::step(uint16 elapsed_cycles)
{

    uint16 cycles = 0;

    while ( elapsed_cycles > cycles )
    {
        div +=  4;
        cycles += 4;

        // If timer is enabled 
        if ( get_bit(2, tac) )
        {
            // Detect falling edge
            bool curr_edge;
            uint8 tima_speed = tac & 0x03;

            if ( tima_speed == Speed::freq_262KHz )
                curr_edge = get_bit(3, div);
            else if ( tima_speed == Speed::freq_65KHz )
                curr_edge = get_bit(5, div);
            else if ( tima_speed == Speed::freq_16KHz )
                curr_edge = get_bit(7, div);
            else // if ( tima_speed == Speed::freq_4KHz )
                curr_edge = get_bit(9, div);

            if ( (prev_edge == true) && (curr_edge == false) )
            {
                tima++;

                // Overflow
                if ( tima == 0x00 )
                {
                    tima = tma;

                    set_bit(2, if_register, true);
                }
            }

            prev_edge = curr_edge;
        }
    }
}
