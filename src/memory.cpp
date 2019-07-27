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
    get_memory_reference(Addr::tima) = 0x00;   // TIMA
    get_memory_reference(Addr::tma)  = 0x00;   // TMA
    get_memory_reference(Addr::tac)  = 0x00;   // TAC
    get_memory_reference(Addr::nr10) = 0x80;   // NR10
    get_memory_reference(Addr::nr11) = 0xBF;   // NR11
    get_memory_reference(Addr::nr12) = 0xF3;   // NR12
    get_memory_reference(Addr::nr14) = 0xBF;   // NR14
    get_memory_reference(Addr::nr21) = 0x3F;   // NR21
    get_memory_reference(Addr::nr22) = 0x00;   // NR22
    get_memory_reference(Addr::nr24) = 0xBF;   // NR24
    get_memory_reference(Addr::nr30) = 0x7F;   // NR30
    get_memory_reference(Addr::nr31) = 0xFF;   // NR31
    get_memory_reference(Addr::nr32) = 0x9F;   // NR32
    get_memory_reference(Addr::nr33) = 0xBF;   // NR33
    get_memory_reference(Addr::nr41) = 0xFF;   // NR41
    get_memory_reference(Addr::nr42) = 0x00;   // NR42
    get_memory_reference(Addr::nr43) = 0x00;   // NR43
    get_memory_reference(Addr::nr44) = 0xBF;   // NR44
    get_memory_reference(Addr::nr50) = 0x77;   // NR50
    get_memory_reference(Addr::nr51) = 0xF3;   // NR51
    get_memory_reference(Addr::nr52) = 0xF1;   // 0xF1-GB 0xF0-SGB // NR52
    get_memory_reference(Addr::lcdc) = 0x91;   // LCDC
    get_memory_reference(Addr::scy)  = 0x00;   // SCY
    get_memory_reference(Addr::scx)  = 0x00;   // SCX
    get_memory_reference(Addr::lyc)  = 0x00;   // LYC
    get_memory_reference(Addr::bgp)  = 0xFC;   // BGP
    get_memory_reference(Addr::obp0) = 0xFF;   // OBP0
    get_memory_reference(Addr::obp1) = 0xFF;   // OBP1
    get_memory_reference(Addr::wy)   = 0x00;   // WY
    get_memory_reference(Addr::wx)   = 0x00;   // WX
    get_memory_reference(Addr::ie)   = 0x00;   // IE

    // Temp???
    get_memory_reference(Addr::ly)      = 0x90; // LY
    get_memory_reference(Addr::div)     = 0x1E;   // DIV High
    get_memory_reference(Addr::div - 1) = 0xA0;  // DIV low
}


// 
uint8 MCU::read_8bit( uint16 address)
{
    return get_memory_reference(address);
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

    // Unused memory
    if ( address < 0xFF00 )
        return false;

    // IO registers
    if ( address < 0xFF80 )
    {
        // Send serial
        if ( (address == MCU::Addr::sc) && (data == 0x81) )
        {
            if ( serial_send_callback )
                serial_send_callback( io_registers[MCU::Addr::sb - 0xFF00] );

            return true;
        }

        // Clear DIV
        if ( address == MCU::Addr::div )
        {
            io_registers[MCU::Addr::div - 0xFF00] = 0x00;
            io_registers[(MCU::Addr::div - 1) - 0xFF00] = 0x00;

            return true;
        }

        io_registers[address - 0xFF00] = data;

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

    // Unused memory
    if ( address < 0xFF00 )
        throw "Error: Unable to get reference to unused memory";

    // IO registers
    if ( address < 0xFF80 )
        return io_registers[address - 0xFF00];

    // High ram
    if ( address <= 0xFFFF )
        return hram[address - 0xFF80];

    throw "Error: Unable to get reference from invalid address";
}


