#ifndef _TYPES_H_
#define _TYPES_H_

#include <array>

typedef char    int8;
typedef short   int16;
typedef int     int32;
typedef long    int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;


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


struct Color
{
    uint8 red;
    uint8 green;
    uint8 blue;

    explicit  Color(uint8 r = 0x00, uint8 g = 0x00, uint8 b = 0x00) :
        red(r),
        green(g),
        blue(b)
    {

    }

    Color(uint32 hex)
    {
        uint8* p_hex = (uint8*) &hex;

        red   = p_hex[2];
        green = p_hex[1];
        blue  = p_hex[0];
    }

    Color( const Color& color) = default;

    static const Color WHITE;
    static const Color LIGHT_GREY;
    static const Color GREY;
    static const Color DARK_GREY;
    static const Color BLACK;

    static const Color GB_WHITE;
    static const Color GB_LIGHT_GREY;
    static const Color GB_DARK_GREY;
    static const Color GB_BLACK;
};


struct RGB_Palette
{
    public:

        RGB_Palette(const RGB_Palette& rgb_palette) = default;

        explicit RGB_Palette(Color white, Color  light_grey, Color dark_grey, Color black) :
            color{white, light_grey, dark_grey, black}
        {

        }
        
        const Color& operator [] (size_t index) const
        {
            return color[index];
        }

        static const RGB_Palette BLACK_AND_WHITE;
        static const RGB_Palette GB_ORIGINAL_GREEN;

    private:

        Color color[4];
};

typedef std::array<Color, 64> PixelArray;

struct Tile
{
    public:

        PixelArray toRGB(uint8 gb_palette = 0xE4, const RGB_Palette& rgb_palette = RGB_Palette::BLACK_AND_WHITE ) const
        {
            size_t index = 0;
            PixelArray pixel_array;

            for( int i = 0 ; i < 16 ; i += 2)
            {
                for( int j = 0 ; j < 8 ; j++ )
                {
                    uint8 mask = 0x80 >> j;

                    uint8 pixel_color = 0;

                    // Least significant bit of current pixel
                    if ( (data[i] & mask) > 0 )
                        pixel_color += 1;

                    // Most significant bit of current pixel
                    if ( (data[i + 1] & mask) > 0 )
                        pixel_color += 2;

                    /*
                    if ( gb_palette != 0xE4 )
                    {
                        if ( pixel_color == 0 )
                            pixel_array[index] = rgb_palette[gb_palette & 0x03];
                        if ( pixel_color == 1 )
                            pixel_array[index] = rgb_palette[(gb_palette >> 2) & 0x03];
                        if ( pixle_color == 2)
                            pixel_array[index] = rgb_palette[(gb_palette >> 4) & 0x03];
                        if ( pixel_color == 3 )
                            pixel_array[index] = rgb_palette[(gb_palette >> 6) & 0x03];
                    }
                    */

                    pixel_array[index] = rgb_palette[pixel_color];

                    index++;
                }
            }

            return pixel_array;
        }

    private:

        uint8 data[16]; 
};

#endif // _TYPES_H_
