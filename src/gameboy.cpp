#include "gameboy.h"

// Constructor/Destructor //

Gameboy::Gameboy() :
    cpu(mcu)
{
    reset();
}
Gameboy::~Gameboy()
{

}

void Gameboy::reset()
{
    mcu.reset();
    cpu.reset();
}

// Cpu functions //
        
Registers Gameboy::get_cpu_registers() const
{
    return cpu.reg;
}
void Gameboy::set_cpu_registers(Registers reg)
{
    cpu.reg = reg;
}

Gameboy* g_gameboy;
