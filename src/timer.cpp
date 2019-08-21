#include "timer.h"

#include "gameboy.h"

Timer::Timer(Gameboy* gameboy) :
    system(*gameboy),
    div_ref( system.mcu.get_memory_reference(MCU::ADDRESS::DIV) ),
    tima( system.mcu.get_memory_reference(MCU::ADDRESS::TIMA) ),
    tma( system.mcu.get_memory_reference(MCU::ADDRESS::TMA) ),
    tac( system.mcu.get_memory_reference(MCU::ADDRESS::TAC) ),
    if_timer_flag( system.mcu.get_memory_reference(MCU::ADDRESS::IF) )
{
    reset();
}

void Timer::reset()
{
    div = 0x0000;
}

void Timer::step(uint16 elapsed_clocks)
{

    uint16 clocks = 0;

    while ( elapsed_clocks > clocks )
    {
        div +=  4;
        clocks += 4;

        div_ref = div_high;

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
