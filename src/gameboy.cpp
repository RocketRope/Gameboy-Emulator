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

void Gameboy::load_rom(const char* filename)
{
    cartridge = Cartridge::load_rom(filename);
}


