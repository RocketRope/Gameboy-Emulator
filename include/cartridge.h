#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include <fstream>
#include <memory>

#include "types.h"

// Cartridge interface class
class Cartridge
{
    public:

        // Rom header
        struct Header
        {
            uint8  logo[48];              // 0x0104 - 0x0133 - Nintendo Logo
            char   title[11];             // 0x0134 - 0x013E - Title
            char   manufacture_code[4];   // 0x013F - 0x0142 - Manufacturer Code
            uint8  CGB_flag;              // 0x0143          - CGB Flag
            uint16 new_license_code;      // 0x0144 - 0x0145 - New License Code
            uint8  SGB_flag;              // 0x0146          - SGB Flag
            uint8  cartridge_type;        // 0x0147          - Cartridge Type
            uint8  rom_size;              // 0x0148          - ROM size
            uint8  ram_size;              // 0x0149          - RAM Size
            uint8  destination_code;      // 0x014A          - Destination Code
            uint8  old_license_code;      // 0x014B          - Old License Code
            uint8  rom_version;           // 0x014C          - Mask ROM Version number
            uint8  header_checksum;       // 0x014D          - Header Checksum
            uint16 global_checksum;       // 0x014E - 0x014F - Global Checksum
        };

        enum MBC_TYPE
        {
            ROM_ONLY                = 0x00,
            ROM_RAM                 = 0x08,
            ROM_RAM_BATTERY         = 0x09,

            MBC1                    = 0x01,
            MBC1_RAM                = 0x02,
            MBC1_RAM_BATTERY        = 0x03,

            MBC2                    = 0x05,
            MBC2_BATTERY            = 0x06,

            MMM01                   = 0x0B,
            MMM01_RAM               = 0x0C,
            MMM01_RAM_BATTERY       = 0x0D,

            MBC3_TIMER_BATTERY      = 0x0F,
            MBC3_TIMER_RAM_BATTERY  = 0x10,
            MBC3                    = 0x11,
            MBC3_RAM                = 0x12,
            MBC3_RAM_BATTERY        = 0x13,

            MBC5                    = 0x019,
            MBC5_RAM                = 0x1A,
            MBC5_RAM_BATTERY        = 0x1B,
            MBC5_RUMBLE             = 0x1C,
            MBC5_RUMBLE_RAM         = 0x1D,
            MBC5_RUMBLE_RAM_BATTERY = 0x1E,

            MBC6                    = 0x20,

            MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
            POCKET_CAMERA           = 0xFC,
            BANDAI_TAMA5            = 0xFD,

            HuC3                    = 0xFE,
            HuC1_RAM_BATTERY        = 0xFF
        };

        // Constructor/Destructor //

        Cartridge( Header _header = {} );
        virtual ~Cartridge();

        // Reset function //
        
        virtual void reset();

        // Virtual Read/Write functions //

        virtual uint8& read_8bit(uint16 address);
        virtual bool   write_8bit(uint16 address, uint8 data);

        // Get functions //
        Header get_header();

        // Static functions //

        static std::unique_ptr<Cartridge> load_rom(const char* filename);
        
    protected:

        Header header;

        std::array<uint8, 0x8000> rom;
        std::array<uint8, 0x2000> ram;
};

class No_MBC : public Cartridge
{
    public:

        // Constructor/Destructor //

        No_MBC(Header _header, std::ifstream& ifs);

        // Override Read/Write functions //
        
        uint8& read_8bit(uint16 address) override;
        bool   write_8bit(uint16 address, uint8 data) override;

    private:
};

#endif
