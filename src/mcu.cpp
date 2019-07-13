#include "mcu.h"

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
    ram.fill(0x00);

    write_8bit(Addr::tima, 0x00);   // TIMA
    write_8bit(Addr::tma,  0x00);   // TMA
    write_8bit(Addr::tac,  0x00);   // TAC
    write_8bit(Addr::nr10, 0x80);   // NR10
    write_8bit(Addr::nr11, 0xBF);   // NR11
    write_8bit(Addr::nr12, 0xF3);   // NR12
    write_8bit(Addr::nr14, 0xBF);   // NR14
    write_8bit(Addr::nr21, 0x3F);   // NR21
    write_8bit(Addr::nr22, 0x00);   // NR22
    write_8bit(Addr::nr24, 0xBF);   // NR24
    write_8bit(Addr::nr30, 0x7F);   // NR30
    write_8bit(Addr::nr31, 0xFF);   // NR31
    write_8bit(Addr::nr32, 0x9F);   // NR32
    write_8bit(Addr::nr33, 0xBF);   // NR33
    write_8bit(Addr::nr41, 0xFF);   // NR41
    write_8bit(Addr::nr42, 0x00);   // NR42
    write_8bit(Addr::nr43, 0x00);   // NR43
    write_8bit(Addr::nr44, 0xBF);   // NR44
    write_8bit(Addr::nr50, 0x77);   // NR50
    write_8bit(Addr::nr51, 0xF3);   // NR51
    write_8bit(Addr::nr52, 0xF1);   // 0xF1-GB 0xF0-SGB // NR52
    write_8bit(Addr::lcdc, 0x91);   // LCDC
    write_8bit(Addr::scy,  0x00);   // SCY
    write_8bit(Addr::scx,  0x00);   // SCX
    write_8bit(Addr::lyc,  0x00);   // LYC
    write_8bit(Addr::bgp,  0xFC);   // BGP
    write_8bit(Addr::obp0, 0xFF);   // OBP0
    write_8bit(Addr::obp1, 0xFF);   // OBP1
    write_8bit(Addr::wy,   0x00);   // WY
    write_8bit(Addr::wx,   0x00);   // WX
    write_8bit(Addr::ie,   0x00);   // IE

    write_8bit(Addr::ly,   0x90);   // IE
}


// 
uint8 MCU::read_8bit( uint16 address)
{
    // Rom
    if ( address < 0x8000 )
        return cartridge->read_8bit(address);

    // 
    if ( address < 0xA000 )
        return ram[address];

    if ( address < 0xC000 )
        return cartridge->read_8bit(address);

    return ram[address];
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
    if ( address < 0x8000 )
            return cartridge->write_8bit(address, data);

    if ( address < 0xA000 )
    {
        ram[address] = data;
        return true;
    }

    if ( address < 0xC000 )
        return cartridge->write_8bit(address, data);

    if ( (address == MCU::Addr::sc) && (data == 0x81) )
    {
        log->debug("%v", read_8bit(MCU::Addr::sb));

        if ( serial_send_callback )
            serial_send_callback( ram[MCU::Addr::sb] );

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

//

void MCU::register_serial_send_callback(std::function<void(uint8)> callback)
{
    serial_send_callback = callback;
}
