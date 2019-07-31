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

        enum ADDRESS : uint16
        {
            // Restart Vector
            RST_00 = 0x0000,
            RST_08 = 0x0008,
            RST_10 = 0x0010,
            RST_18 = 0x0018,
            RST_20 = 0x0020,
            RST_28 = 0x0028,
            RST_30 = 0x0030,
            RST_38 = 0x0038,

            // Interupt Vector
            INTERUPT_VBLANK = 0x0040,
            INTERUPT_LCD    = 0x0048,
            INTERUPT_TIMER  = 0x0050,
            INTERUPT_SERIAL = 0x0058,
            INTERUPT_JOYPAD = 0x0060,

            // JOYPAD Input Register
            JOYP = 0xFF00,

            // Serial Data Transfer Registers
            SB = 0xFF01,
            SC = 0xFF02,

            // Timer and Divider Registers
            DIV  = 0xFF04,
            TIMA = 0xFF05,
            TMA  = 0xFF06,
            TAC  = 0xFF07,

            // Sound I/O Registers
            NR10 = 0xFF10,
            NR11 = 0xFF11,
            NR12 = 0xFF12,
            NR13 = 0xFF13,
            NR14 = 0xFF14,

            NR21 = 0xFF16,
            NR22 = 0xFF17,
            NR23 = 0xFF18,
            NR24 = 0xFF19,

            NR30 = 0xFF1A,
            NR31 = 0xFF1B,
            NR32 = 0xFF1C,
            NR33 = 0xFF1D,
            NR34 = 0xFF1E,

            NR41 = 0xFF20,
            NR42 = 0xFF21,
            NR43 = 0xFF22,
            NR44 = 0xFF23,

            NR50 = 0xFF24,
            NR51 = 0xFF25,
            NR52 = 0xFF26,

            // Video I/O Registers
            LCDC = 0xFF40,
            STAT = 0xFF41,
            SCY  = 0xFF42,
            SCX  = 0xFF43,
            LY   = 0xFF44,
            LYC  = 0xFF45,
            DMA  = 0xFF46,
            BGP  = 0xFF47,
            OBP0 = 0xFF48,
            OBP1 = 0xFF49,
            WY   = 0xFF4A,
            WX   = 0xFF4B,
            
            // CGB Video I/O Registers
            VBK   = 0xFF4F,
            HDMA1 = 0xFF51,
            HDMA2 = 0xFF52,
            HDMA3 = 0xFF53,
            HDMA4 = 0xFF54,
            HDMA5 = 0xFF55,
            BCPS  = 0xFF68,
            BCPD  = 0xFF69,
            OCPS  = 0xFF6A,
            OCPD  = 0xFF6B,

            // CGB Registers
            KEY1 = 0xFF4D,
            RP   = 0xFF56,
            SVBK = 0xFF70,

            // Interrupts
            IF = 0xFF0F,
            IE = 0xFFFF
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
