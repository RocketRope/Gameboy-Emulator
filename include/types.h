#ifndef _TYPES_H_
#define _TYPES_H_

#include <array>

// Typedef of regular integer types //

typedef char    int8;
typedef short   int16;
typedef int     int32;
typedef long    int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

// Bit manipulation helper functions //

template <typename T>
bool get_bit(uint8 n, T source)
{
    if ( ( (source >> n) & 1 ) == 1 )
        return true;
    else
        return false;
}

template <typename T>
void set_bit(uint8 n, T& source, bool value = true)
{
    if ( value )
        source |= 0x01 << n;
    else
        source &= ~(0x01 << n);
}

// Bit manipulation class //

template <int B, class T = uint8>
class Bit
{
    public:

        explicit Bit(T& _byte) :
            byte(_byte)
        {

        }

        operator bool () const 
        { 
            return (byte & mask) != 0;
        }

        Bit& operator = (const bool& rhs)
        {
            if ( rhs )
                byte |= mask;
            else
                byte &= ~mask;

            return *this;
        }

    private:

        T& byte;
        const T mask = 0x01 << B;
};

// 24bit RGB Color struct //

struct RGB_Color
{
    public:

        uint8 red;
        uint8 green;
        uint8 blue;

        explicit  RGB_Color(uint8 r = 0x00, uint8 g = 0x00, uint8 b = 0x00) :
            red(r),
            green(g),
            blue(b)
        {

        }

        RGB_Color(int32 hex)
        {
            uint8* p_hex = (uint8*) &hex;

            red   = p_hex[2];
            green = p_hex[1];
            blue  = p_hex[0];
        }

        RGB_Color( const RGB_Color& color) = default;

        static const RGB_Color WHITE;
        static const RGB_Color LIGHT_GREY;
        static const RGB_Color GREY;
        static const RGB_Color DARK_GREY;
        static const RGB_Color BLACK;

        static const RGB_Color GB_WHITE;
        static const RGB_Color GB_LIGHT_GREY;
        static const RGB_Color GB_DARK_GREY;
        static const RGB_Color GB_BLACK;
};

// 24bit RGB Palette struct //

struct RGB_Palette
{
    public:

        RGB_Palette(const RGB_Palette& rgb_palette) = default;

        explicit RGB_Palette( RGB_Color white,
                              RGB_Color light_grey, 
                              RGB_Color dark_grey, 
                              RGB_Color black) :
            color{white, light_grey, dark_grey, black}
        {

        }
        
        const RGB_Color& operator [] (size_t index) const
        {
            return color[index];
        }

        static const RGB_Palette BLACK_AND_WHITE;
        static const RGB_Palette GB_ORIGINAL_GREEN;

    private:

        RGB_Color color[4];
};

// Tile helper class //

typedef RGB_Color Tile_Pixel_Array[64];

struct Tile
{
    public:

        uint8 getPixel(uint8 x, uint8 y);

        void toRGB( Tile_Pixel_Array pixels,
                    uint8 gb_palette = 0xE4, 
                    const RGB_Palette& rgb_palette = RGB_Palette::BLACK_AND_WHITE 
                  ) const;

    private:

        uint8 data[16]; 
};

// Sprite (OBJ - Object) helper class //

struct Object
{
    public:

        enum ATTRIBUTE : uint8
        {
            PRIORITY   = 7,
            FLIP_Y     = 6,
            FLIP_X     = 5,
            GB_PALETTE = 4,
            TILE_BANK  = 3
        };

        bool get_attribute(ATTRIBUTE bit)
        {
            return get_bit(bit, attributes);
        }

        void set_attribute(ATTRIBUTE bit, bool value)
        {
            set_bit(bit, attributes, value);
        }

        uint8 get_cgb_palette()
        {
            return attributes & 0x07;
        }

        void set_cgb_palette(uint8 palette)
        {
            attributes &= 0xF8;
            attributes |= (palette & 0x07);
        }

        uint8 pos_y;
        uint8 pos_x;
        uint8 tile_number;
        uint8 attributes;

    private:

        
};

typedef Object Sprite;

#endif // _TYPES_H_
