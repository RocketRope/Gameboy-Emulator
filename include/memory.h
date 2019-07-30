#ifndef _MCU_H_
#define _MCU_H_

// std
#include <array>
#include <functional>

// el
#include "el/easylogging++.h"

// gbe
#include "types.h"
#include "cartridge.h"

class MCU 
{
    public:

        // Addresses
        enum Addr : uint16
        {
            // Restart vector
            rst_0x00 = 0x0000,
            rst_0x08 = 0x0008,
            rst_0x10 = 0x0010,
            rst_0x18 = 0x0018,
            rst_0x20 = 0x0020,
            rst_0x28 = 0x0028,
            rst_0x30 = 0x0030,
            rst_0x38 = 0x0038,

            // Interupt vector
            interupt_vblank = 0x0040,
            interupt_lcd    = 0x0048,
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

        // Video ram (Tile + BG maps), 2 banks in cgb mode. 0x8000 - 0x9FFF
        uint8 vram[2][0x2000] = {0};

        const Tile*  tile_map_0 = (Tile*) &vram[0][0];
        const Tile*  tile_map_1 = (Tile*) &vram[1][0];

        uint8* bg_map_0 = &vram[0][0x1800];
        uint8* bg_map_1 = &vram[0][0x1C00];

        // Work ram, 8 banks in cgb mode 0xC000 - 0xDFFF
        uint8 wram[8][0x1000] = {0};

        // Sprite Attribute Table (OAM) ram 0xFE00 - 0xFE9F
        uint8 oam[0x00A0] = {0};

        // IO registers + Unused memory. 0xFEA0 - 0xFF7F
        uint8 io_registers[0x00E0] = {0};

        // High ram + ie register. 0xFF80 - 0xFFFF 
        uint8 hram[0x0080] = {0};

        // Constructor/Destructor //
        MCU(std::unique_ptr<Cartridge>& _cartridge);
        ~MCU();

        //
        void reset();

        //
        uint8  read_8bit( uint16 address);
        uint16 read_16bit(uint16 address);

        //
        bool write_8bit( uint16 address, uint8  data);
        bool write_16bit(uint16 address, uint16 data);

        // 
        uint8&  get_memory_reference(uint16 address);

        // Callback register functions 
        void register_serial_send_callback(std::function<void(uint8)> callback);
        void register_input_read_callback(std::function<void(void)> callback);

    private:

        el::Logger* log;

        // Cartridge Rom (0x0000 - 0x7FFF) + Ram (0xA000 - 0xBFFF)
        std::unique_ptr<Cartridge>& cartridge;

        // 
        uint8 current_vram_bank = 0;
        uint8 current_wram_bank = 1;

        // Callback functions variables
        std::function<void(uint8)> serial_send_callback;
        std::function<void(void)>  joypad_read_callback;

    // Friends

    friend class Timer;
    friend class LR35902;
};

#endif // _MCU_H_
