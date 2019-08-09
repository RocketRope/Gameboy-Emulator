#include "memory.h"

// Constructor
MCU::MCU(std::unique_ptr<Cartridge>& _cartridge) :
    cartridge(_cartridge)
{
    log = el::Loggers::getLogger("MCU");

    reset();
}

MCU::~MCU()
{    
    el::Loggers::unregisterLogger("MCU");
}

//
void MCU::reset()
{
    // Reset Video ram
    for ( int bank = 0 ; bank < 1 ; bank++ )
        for ( int i = 0 ; i < 0x2000 ; i++ )
            vram[bank][i] = 0x00;

    // Reset Work ram
    for ( int bank = 0 ; bank < 8 ; bank++ )
        for ( int i = 0 ; i < 0x1000 ; i++ )
            wram[bank][i] = 0x00;

    // Reset OAM
    for ( int i = 0 ; i < 0x00A0 ; i++ )
        oam[i] = 0x00;

    // Reset IO Registers
    for ( int i = 0 ; i < 0x00E0 ; i++ )
        io_registers[i] = 0x00;

    for ( int i = 0 ; i < 0x005F ; i++ )
        io_registers[i] = 0xFF;

    // Reset hram
    for ( int i = 0 ; i < 0x0080 ; i ++)
        hram[i] = 0x00;

    get_memory_reference(ADDRESS::TIMA) = 0x00;   // TIMA
    get_memory_reference(ADDRESS::TMA)  = 0x00;   // TMA
    get_memory_reference(ADDRESS::TAC)  = 0x00;   // TAC
    get_memory_reference(ADDRESS::NR10) = 0x80;   // NR10
    get_memory_reference(ADDRESS::NR11) = 0xBF;   // NR11
    get_memory_reference(ADDRESS::NR12) = 0xF3;   // NR12
    get_memory_reference(ADDRESS::NR14) = 0xBF;   // NR14
    get_memory_reference(ADDRESS::NR21) = 0x3F;   // NR21
    get_memory_reference(ADDRESS::NR22) = 0x00;   // NR22
    get_memory_reference(ADDRESS::NR24) = 0xBF;   // NR24
    get_memory_reference(ADDRESS::NR30) = 0x7F;   // NR30
    get_memory_reference(ADDRESS::NR31) = 0xFF;   // NR31
    get_memory_reference(ADDRESS::NR32) = 0x9F;   // NR32
    get_memory_reference(ADDRESS::NR33) = 0xBF;   // NR33
    get_memory_reference(ADDRESS::NR41) = 0xFF;   // NR41
    get_memory_reference(ADDRESS::NR42) = 0x00;   // NR42
    get_memory_reference(ADDRESS::NR43) = 0x00;   // NR43
    get_memory_reference(ADDRESS::NR44) = 0xBF;   // NR44
    get_memory_reference(ADDRESS::NR50) = 0x77;   // NR50
    get_memory_reference(ADDRESS::NR51) = 0xF3;   // NR51
    get_memory_reference(ADDRESS::NR52) = 0xF1;   // 0XF1-GB 0xF0-SGB // NR52
    get_memory_reference(ADDRESS::LCDC) = 0x91;   // LCDC
    get_memory_reference(ADDRESS::SCY)  = 0x00;   // SCY
    get_memory_reference(ADDRESS::SCX)  = 0x00;   // SCX
    get_memory_reference(ADDRESS::LYC)  = 0x00;   // LYC
    get_memory_reference(ADDRESS::BGP)  = 0xFC;   // BGP
    get_memory_reference(ADDRESS::OBP0) = 0xFF;   // OBP0
    get_memory_reference(ADDRESS::OBP1) = 0xFF;   // OBP1
    get_memory_reference(ADDRESS::WY)   = 0x00;   // WY
    get_memory_reference(ADDRESS::WX)   = 0x00;   // WX
    get_memory_reference(ADDRESS::IE)   = 0x00;   // IE

    // TEMP???
    get_memory_reference(ADDRESS::LY)      = 0x00; // LY
    get_memory_reference(ADDRESS::DIV)     = 0x1E;   // DIV High
    get_memory_reference(ADDRESS::DIV - 1) = 0xA0;  // DIV low
}


// 
uint8 MCU::read_8bit( uint16 address)
{
    switch (address)
    {
        case MCU::ADDRESS::JOYP:
        {
            uint8& joyp = get_memory_reference(address);

            if ( get_bit(4, joyp) == false )
            {
                set_bit(0, joyp, !joypad.right);
                set_bit(1, joyp, !joypad.left);
                set_bit(2, joyp, !joypad.up);
                set_bit(3, joyp, !joypad.down);
            }
            else if ( get_bit(5, joyp) == false )
            {
                set_bit(0, joyp, !joypad.a);
                set_bit(1, joyp, !joypad.b);
                set_bit(2, joyp, !joypad.select);
                set_bit(3, joyp, !joypad.start);
            }

            return joyp;
        }

        default:
            return get_memory_reference(address);
    }
    
}
uint16 MCU::read_16bit(uint16 address)
{
    uint8 low  = read_8bit(address + 1);
    uint8 high = read_8bit(address);

    uint16 out  = (low << 8) + high;

    return out;
}

//
bool MCU::write_8bit( uint16 address, uint8  data)
{

    // Rom
    if ( address < 0x8000 )
       return  cartridge->write_8bit(address, data);

    // Vram
    if ( address < 0xA000 )
    {
        vram[current_vram_bank][address - 0x8000] = data;
        return true;
    }
    
    // Ext Ram
    if ( address < 0xC000 )
        return cartridge->write_8bit(address, data);

    // Wram 
    if ( address < 0xD000 ) // Fixed bank
    {
        wram[0][address - 0xC000] = data;
        return true;
    }
    if ( address < 0xE000 ) // Switchable bank
    {
        wram[current_wram_bank][address - 0xD000] = data;
        return true;
    }

    // Echo ram
    if ( address < 0xF000 ) // Fixed bank
    {
        wram[0][address - 0xE000] = data;
        return true;
    }
    if ( address < 0xFE00 ) // Switchable bank
    {
        wram[current_wram_bank][address - 0xF000] = data;
        return true;
    }

    // Sprite Attribute Table
    if ( address < 0xFEA0 )
    {
        oam[address - 0xFE00] = data;
        return true;
    }

    // IO registers
    if ( address < 0xFF80 )
    {
        switch (address)
        {
            case MCU::ADDRESS::SC:
            {
                if ( data == 0x81 )
                    if ( serial_send_callback )
                        serial_send_callback( io_registers[MCU::ADDRESS::SB - 0xFEA0] );

                return true;
            }

            case MCU::ADDRESS::DIV:
            {
                io_registers[MCU::ADDRESS::DIV - 0xFEA0] = 0x00;
                io_registers[(MCU::ADDRESS::DIV - 1) - 0xFEA0] = 0x00;

                return true;
            }

            case MCU::ADDRESS::DMA:
            {
                dma(data);
                
                return true;
            }

            case MCU::ADDRESS::JOYP:
            {
                io_registers[address - 0xFEA0] = data & 0x30;

                return true;
            }
            case MCU::ADDRESS::SB:
            case MCU::ADDRESS::TIMA:
            case MCU::ADDRESS::TMA:
            case MCU::ADDRESS::TAC:
            case MCU::ADDRESS::NR10:
            case MCU::ADDRESS::NR11:
            case MCU::ADDRESS::NR12:
            case MCU::ADDRESS::NR13:
            case MCU::ADDRESS::NR14:
            case MCU::ADDRESS::NR21:
            case MCU::ADDRESS::NR22:
            case MCU::ADDRESS::NR23:
            case MCU::ADDRESS::NR24:
            case MCU::ADDRESS::NR30:
            case MCU::ADDRESS::NR31:
            case MCU::ADDRESS::NR32:
            case MCU::ADDRESS::NR33:
            case MCU::ADDRESS::NR34:
            case MCU::ADDRESS::NR41:
            case MCU::ADDRESS::NR42:
            case MCU::ADDRESS::NR43:
            case MCU::ADDRESS::NR44:
            case MCU::ADDRESS::NR50:
            case MCU::ADDRESS::NR51:
            case MCU::ADDRESS::NR52:
            case MCU::ADDRESS::LCDC:
            case MCU::ADDRESS::STAT:
            case MCU::ADDRESS::SCY:
            case MCU::ADDRESS::SCX:
            case MCU::ADDRESS::LY:
            case MCU::ADDRESS::LYC:
            case MCU::ADDRESS::BGP:
            case MCU::ADDRESS::OBP0:
            case MCU::ADDRESS::OBP1:
            case MCU::ADDRESS::WY:
            case MCU::ADDRESS::WX:
            default:
                io_registers[address - 0xFEA0] = data;
        }

        return true;
    }

    // High ram
    if ( address <= 0xFFFF )
    {
        hram[address - 0xFF80] = data;
        return true;
    }

    return false;
}
bool MCU::write_16bit(uint16 address, uint16 data)
{
    uint8 low  = data;
    uint8 high = data >> 8;

    bool low_ok  = write_8bit(address, low);
    bool high_ok = write_8bit(address + 1, high);

    return low_ok && high_ok;
}

// Callback functions

void MCU::register_serial_send_callback(std::function<void(uint8)> callback)
{
    serial_send_callback = callback;
}

// 

uint8&  MCU::get_memory_reference(uint16 address)
{
    // Rom
    if ( address < 0x8000 )
        return cartridge->read_8bit(address);

    // Vram
    if ( address < 0xA000 )
        return vram[current_vram_bank][address - 0x8000];

    // Ext Ram
    if ( address < 0xC000 )
        return cartridge->read_8bit(address);

    // Wram 
    if ( address < 0xD000 ) // Fixed bank
        return wram[0][address - 0xC000];
    if ( address < 0xE000 ) // Switchable bank
        return wram[current_wram_bank][address - 0xD000];

    // Echo ram
    if ( address < 0xF000 ) // Fixed bank
        return wram[0][address - 0xE000];
    if ( address < 0xFE00 ) // Switchable bank
        return wram[current_wram_bank][address - 0xF000];

    // Sprite Attribute Table
    if ( address < 0xFEA0 )
        return oam[address - 0xFE00];

    // IO registers
    if ( address < 0xFF80 )
        return io_registers[address - 0xFEA0];

    // High ram
    if ( address <= 0xFFFF )
        return hram[address - 0xFF80];

    throw "Error: Unable to get reference from invalid address";
}

// DMA transfer function
void MCU::dma(uint8 source)
{
    uint16 address = static_cast<uint16>( source << 8 );

    for ( uint16 i = 0 ; i < 0x00A0 ; i++ )
        oam[i] = get_memory_reference(address + i);
}


