#include "timer.h"

Timer::Timer(MCU* _mcu) :
    mcu(_mcu),
    div( *((uint16*) (&(mcu->get_memory_reference(MCU::ADDRESS::DIV - 1)))) ),
    div_low( mcu->get_memory_reference(0xFF03) ),
    div_high( mcu->get_memory_reference(MCU::ADDRESS::DIV) ),
    tima( mcu->get_memory_reference(MCU::ADDRESS::TIMA) ),
    tma( mcu->get_memory_reference(MCU::ADDRESS::TMA) ),
    tac( mcu->get_memory_reference(MCU::ADDRESS::TAC) ),
    if_timer_flag( mcu->get_memory_reference(MCU::ADDRESS::IF) )
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

            if ( tima_speed == SPEED::FREQ_262kHz )
                curr_edge = get_bit(3, div);
            else if ( tima_speed == SPEED::FREQ_65kHz )
                curr_edge = get_bit(5, div);
            else if ( tima_speed == SPEED::FREQ_16kHz )
                curr_edge = get_bit(7, div);
            else // if ( tima_speed == SPEED::FREQ_4kHz )
                curr_edge = get_bit(9, div);

            if ( (prev_edge == true) && (curr_edge == false) )
            {
                tima++;

                // Overflow
                if ( tima == 0x00 )
                {
                    tima = tma;

                    // Request interrupt
                    if_timer_flag = true;
                }
            }

            prev_edge = curr_edge;
        }
    }
}
