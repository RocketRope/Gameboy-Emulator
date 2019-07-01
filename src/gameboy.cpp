#include "gameboy.h"

// Global gameboy pointer
Gameboy* g_gameboy;

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

bool Gameboy::get_cpu_flag(CPU::Flag flag)
{
    return cpu.get_flag(flag);
}
void Gameboy::set_cpu_flag(CPU::Flag flag, bool value)
{
    cpu.set_flag(flag, value);
}

