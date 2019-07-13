#include "gameboy.h"

// Global gameboy pointer
Gameboy* g_gameboy;

// Constructor/Destructor //

Gameboy::Gameboy() :
    mcu(cartridge),
    cpu(&mcu)
{
    cartridge = std::make_unique<Cartridge>();
}
Gameboy::~Gameboy()
{

}

void Gameboy::reset()
{
    mcu.reset();
    cpu.reset();
}

void Gameboy::run(int cycles, uint16 break_pc)
{
    for ( int i = 0 ; i < cycles ; i++ )
    {
        cpu.step();
        
        if ( cpu.reg.pc == break_pc )
            return;
    }
}

void Gameboy::load_rom(const char* filename)
{
    cartridge = Cartridge::load_rom(filename);
}


