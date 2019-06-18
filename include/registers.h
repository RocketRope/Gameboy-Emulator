#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <iomanip>
#include <iostream>

#include "types.h"

struct Registers
{
    union // AF
    {
        uint16 af;
        struct { uint8 f, a; };
    };

    union // BC
    {
        uint16 bc;
        struct { uint8 c, b; };
    };

    union // DE
    {
        uint16 de;
        struct { uint8 e, d; };
    };

    union // HL
    {
        uint16 hl;
        struct { uint8 l, h; };
    };

    union // Program counter
    {
        uint16 pc;
        struct { uint8 pc_low, pc_high; };
    };

    union // Stack pointer
    {
        uint16 sp;
        struct { uint8 sp_low, sp_high; };
    };

    friend std::ostream& operator << (std::ostream& os, const Registers& reg)
    {
        auto fill  = os.fill('0');
        auto flags = os.flags( std::ios::hex | std::ios::uppercase );

        os << "af: 0x" << std::setw(4)  << reg.af << " - ";
        os << "bc: 0x" << std::setw(4)  << reg.bc << " - ";

        os << "de: 0x" << std::setw(4)   << reg.de << " - ";
        os << "hl: 0x" << std::setw(4)   << reg.hl << " - ";

        os << "pc: 0x" << std::setw(4)   << reg.pc << " - ";
        os << "sp: 0x" << std::setw(4)   << reg.sp;

        os.fill(fill);
        os.flags(flags);

        return os;
    }
};

#endif // _REGISTERS_H_