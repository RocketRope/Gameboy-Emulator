#ifndef _GAMEBOY_H_
#define _GAMEBOY_H_

#include <iostream>

#include "el/easylogging++.h"

#include "cpu.h"
#include "timer.h"
#include "memory.h"
#include "cartridge.h"

typedef LR35902 CPU;

class Gameboy
{
    public:

        MCU mcu;
        LR35902 cpu;
        Timer timer;
        std::unique_ptr<Cartridge> cartridge;
        
        // Constructor/Destructor //

        Gameboy();
        ~Gameboy();

        void reset();

        void run(int cycles, uint16 break_pc = 0x0000);

        void load_rom(const char* filename);

    private:

};

#endif // _GAMEBOY_H_
