#ifndef _MCU_H_
#define _MCU_H_

// std
#include <array>

// el
#include "el/easylogging++.h"

// gbe
#include "types.h"

class MCU 
{
    public:

        // Addresses
        enum Addr : uint16
        {
            // Restart vector addresses
            rst_0x00 = 0x0000,
            rst_0x08 = 0x0008,
            rst_0x10 = 0x0010,
            rst_0x18 = 0x0018,
            rst_0x20 = 0x0020,
            rst_0x28 = 0x0028,
            rst_0x30 = 0x0030,
            rst_0x38 = 0x0038,

            // Interupt vector addresses
            interupt_vblank = 0x0040,
            interupt_lcdc   = 0x0048,
            interupt_timer  = 0x0050,
            interupt_serial = 0x0058,
            interupt_joypad = 0x0060,

            // Joypad input register
            joyp = 0xFF00,

            // Serial data transfer registers
            sb = 0xFF01,
            sc = 0xFF02,

            // Timer and divider registers
            div  = 0xFF04,
            tima = 0xFF05,
            tma  = 0xFF06,
            tac  = 0xFF07,

            // Sound I/O registers
            nr10 = 0xFF10,
            nr11 = 0xFF11,
            nr12 = 0xFF12,
            nr13 = 0xFF13,
            nr14 = 0xFF14,

            nr21 = 0xFF16,
            nr22 = 0xFF17,
            nr23 = 0xFF18,
            nr24 = 0xFF19,

            nr30 = 0xFF1A,
            nr31 = 0xFF1B,
            nr32 = 0xFF1C,
            nr33 = 0xFF1D,
            nr34 = 0xFF1E,

            nr41 = 0xFF20,
            nr42 = 0xFF21,
            nr43 = 0xFF22,
            nr44 = 0xFF23,

            nr50 = 0xFF24,
            nr51 = 0xFF25,
            nr52 = 0xFF26,

            // Video I/O Registers
            lcdc = 0xFF40,
            stat = 0xFF41,
            scy  = 0xFF42,
            scx  = 0xFF43,
            ly   = 0xFF44,
            lyc  = 0xFF45,
            dma  = 0xFF46,
            bgp  = 0xFF47,
            obp0 = 0xFF48,
            obp1 = 0xFF49,
            wy   = 0xFF4A,
            wx   = 0xFF4B,
            
            // CGB Video I/O registers
            vbk   = 0xFF4F,
            hdma1 = 0xFF51,
            hdma2 = 0xFF52,
            hdma3 = 0xFF53,
            hdma4 = 0xFF54,
            hdma5 = 0xFF55,
            bcps  = 0xFF68,
            bcpd  = 0xFF69,
            ocps  = 0xFF6A,
            ocpd  = 0xFF6B,

            // CGB registers
            key1 = 0xFF4D,
            rp   = 0xFF56,
            svbk = 0xFF70,

            // Interrupts
            if_ = 0xFF0F,
            ie = 0xFFFF
        };

        // Constructor/Destructor //
        MCU();
        ~MCU();

        //
        void reset();

        //
        bool loadCartridge(const char* filename);
        
        //
        uint8  read_8bit( uint16 address);
        uint16 read_16bit(uint16 address);

        //
        void write_8bit( uint16 address, uint8  data);
        void write_16bit(uint16 address, uint16 data);

    private:

        el::Logger* log;

        std::array<uint8, 0xFFFF> ram;
};

#endif // _MCU_H_