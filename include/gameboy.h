#ifndef _GAMEBOY_H_
#define _GAMEBOY_H_

#include "el/easylogging++.h"

#include "cpu.h"
#include "mcu.h"

class Gameboy
{
    public:

        MCU mcu;
        LR35902 cpu;

        // Constructor/Destructor //

        Gameboy();
        ~Gameboy();

        void reset();

    private:
};

#endif // _GAMEBOY_H_
