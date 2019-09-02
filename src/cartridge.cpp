#include "cartridge.h"

#include <iterator>
#include <algorithm>

// Constructor/Destructor //

Cartridge::Cartridge(Header _header) :
    header(_header)
{

}
Cartridge::~Cartridge()
{

}

// Reset function //

void Cartridge::reset()
{
    ram.fill(0x00);
}

//

bool Cartridge::is_empty()
{
    return dynamic_cast<Empty_Cartridge*>( this ) != nullptr;
}

// Virtual Read/Write functions //

uint8& Cartridge::read_8bit(uint16 /*address*/)
{
    return rom[0x000];
}
bool Cartridge::write_8bit(uint16 /*address*/, uint8 /*data*/)
{
    return false;
}

Cartridge::Header Cartridge::get_header()
{
    return header;
}

// Static functions //

std::unique_ptr<Cartridge> Cartridge::load_rom(const std::string& filename)
{
    Header header;
    std::ifstream file;
    
    file.open(filename, std::ios::binary);

    if ( !file.is_open() )
        return std::make_unique<Empty_Cartridge>();

    // Go to start of rom header
    file.seekg(0x0104);

    // Read header
    file.read((char*) &header, sizeof(Header));

    // Rewind
    file.seekg(0x0000);

    switch ( header.cartridge_type )
    {

    case MBC_TYPE::ROM_ONLY :
    case MBC_TYPE::ROM_RAM  :
    case MBC_TYPE::ROM_RAM_BATTERY :
        return std::make_unique<No_MBC>(header, file);

    case MBC_TYPE::MBC1:
        return std::make_unique<No_MBC>(header, file); // Temp!!!!!

    default:
        return std::make_unique<Empty_Cartridge>();
    }
}

// Sub Class No_MBC //

No_MBC::No_MBC(Header _header, std::ifstream& ifs) :
    Cartridge(_header)
{
    ifs.read((char*) rom.data(), 0x8000);
}

// Override Read/Write functions //

uint8& No_MBC::read_8bit(uint16 address)
{
    // Rom
    if ( address < 0x8000 )
        return rom[address];

    // Invalid cartridge address
    if ( address < 0xA000 )
        return rom[0x000];
        
    // Ram
    if ( address < 0xC000 )
        return ram[address - 0xA000];

    // Invalid cartridge address
    return rom[0x000];
}
bool  No_MBC::write_8bit(uint16 address, uint8 data)
{
    if ( address < 0xA000 )
        return false;
        
    if ( address < 0xC000 )
    {
        ram[address - 0xA000] = data;
        return true;
    }

    return false;
}
