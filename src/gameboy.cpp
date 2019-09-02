#include "gameboy.h"

#include <functional>

// Constructor/Destructor //

Gameboy::Gameboy() :
    mcu(this),
    cpu(this),
    timer(this),
    ppu(this)
{
    cartridge = std::make_unique<Empty_Cartridge>();

    exit = false;
    pause();

    run_thread = std::thread(std::bind(&Gameboy::run_loop, this));
}
Gameboy::~Gameboy()
{
    exit = true;

    running = true;
    run_cv.notify_one();
    run_thread.join();
}

void Gameboy::reset()
{
    mcu.reset();
    cpu.reset();
    ppu.reset();
    timer.reset();

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
    if ( !cartridge->is_empty() )
    {
        running = true;
        run_cv.notify_one();
    }
}

void Gameboy::pause()
{
    running = false;
}

bool Gameboy::load_rom(const std::string& filename)
{
    auto new_cartridge = Cartridge::load_rom(filename);

    if ( new_cartridge->is_empty() )
        return false;

    cartridge = std::move(new_cartridge);

    reset();

    return true;
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

