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

    mcu.loadCartridge("../tests/game.gb");
}