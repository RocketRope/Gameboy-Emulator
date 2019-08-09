#include "gameboy.h"

// Constructor/Destructor //

Gameboy::Gameboy() :
    mcu(cartridge),
    cpu(&mcu),
    timer(&mcu),
    ppu(&mcu)
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
    ppu.reset();
    timer.reset();
}

void Gameboy::run(int clocks, uint16 break_pc)
{
    for ( int i = 0 ; i < clocks ; i++ )
    {
        uint64 elapsed_clocks = cpu.step();
        
        timer.step(elapsed_clocks);

        ppu.step(elapsed_clocks);

        if ( cpu.reg.pc == break_pc )
            return;
    }
}

void Gameboy::load_rom(const char* filename)
{
    cartridge = Cartridge::load_rom(filename);
}


