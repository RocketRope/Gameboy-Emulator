#ifndef _GAMEBOY_H_
#define _GAMEBOY_H_

#include "el/easylogging++.h"

#include "cpu.h"
#include "mcu.h"

typedef LR35902 CPU;

class Gameboy
{
    public:

        // Constructor/Destructor //

        Gameboy();
        ~Gameboy();

        void reset();

        // Cpu functions //

        Registers get_cpu_registers() const;
        void      set_cpu_registers(Registers reg);

        bool get_cpu_flag(CPU::Flag flag);
        void set_cpu_flag(CPU::Flag flag, bool value = true);

        // Mcu functions //

        uint8  memory_read_8bit(uint16 address);
        uint16 memory_read_16bit(uint16 address);

        void memory_write_8bit(uint16 address, uint8 data);
        void memory_write_16bit(uint16 address, uint16 data); 

    private:

        MCU mcu;
        LR35902 cpu;
};

// Global gameboy pointer
extern Gameboy* g_gameboy;

#endif // _GAMEBOY_H_
