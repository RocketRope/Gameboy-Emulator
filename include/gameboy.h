#ifndef _GAMEBOY_H_
#define _GAMEBOY_H_

#include "el/easylogging++.h"

#include "cpu.h"
#include "mcu.h"

class Gameboy
{
    public:


        // Constructor/Destructor //

        Gameboy();
        ~Gameboy();

        void reset();

        // Cpu functions //
        Registers get_cpu_registers() const;
        void      set_cpu_registers();
        
    private:

        MCU mcu;
        LR35902 cpu;
};

#endif // _GAMEBOY_H_
