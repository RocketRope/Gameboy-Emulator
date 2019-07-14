#include "gameboy.h"

// Constructor/Destructor //

Gameboy::Gameboy() :
    mcu(cartridge),
    cpu(&mcu),
    timer(&mcu)
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
        uint64 elapsed_cycles = cpu.step();
        
        timer.step(elapsed_cycles);

        if ( cpu.reg.pc == break_pc )
            return;
    }
}

void Gameboy::load_rom(const char* filename)
{
    cartridge = Cartridge::load_rom(filename);
}


