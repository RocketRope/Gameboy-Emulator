#include "gameboy.h"

#include <functional>

// Constructor/Destructor //

Gameboy::Gameboy() :
    mcu(cartridge),
    cpu(&mcu),
    timer(&mcu),
    ppu(&mcu)
{
    cartridge = std::make_unique<Cartridge>();

    exit = false;
    pause();

    run_thread = std::thread(std::bind(&Gameboy::run_loop, this));
}
Gameboy::~Gameboy()
{
    exit = true;
    run();

    run_thread.join();
}

void Gameboy::reset()
{
    mcu.reset();
    cpu.reset();
    ppu.reset();
    timer.reset();

    if ( cartridge != nullptr )
        cartridge->reset();
}

void Gameboy::step()
{
    uint64 elapsed_clocks = cpu.step();
        
    timer.step(elapsed_clocks);

    ppu.step(elapsed_clocks);
}

void Gameboy::run()
{
    running = true;
    run_cv.notify_one();
}

void Gameboy::pause()
{
    running = false;
}

void Gameboy::load_rom(const char* filename)
{
    reset();

    cartridge = Cartridge::load_rom(filename);
}

void Gameboy::run_loop()
{
    while( exit == false )
    {
        if ( running == true )
        {
            step();
        }
        else
        {
            std::unique_lock lock(run_mutex);
            run_cv.wait(lock, [this](){ return running == true; });
        }
    }
}

