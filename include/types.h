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


struct RGB_Palette
{
    public:

        static const RGB_Palette BLACK_AND_WHITE;

        RGB_Palette(const RGB_Palette& rhs) = default;

        explicit RGB_Palette(uint8 white, uint8 light_grey, uint8 dark_grey, uint8 black) :
            color{white, light_grey, dark_grey, black}
        {

        }
        
        const uint8& operator [] (size_t index) const
        {
            return color[index];       
        }

    private:

        uint8 color[4];
};



struct Tile
{
    public:

        std::array<uint8, 64> toRGB(uint8 gb_palette = 0xE4, const RGB_Palette& rgb_palette = RGB_Palette::BLACK_AND_WHITE )
        {
            size_t index = 0;
            std::array<uint8, 64> pixel_array;

            for( int i = 0 ; i < 16 ; i += 2)
            {
                uint8 mask = 0x80;

                for( int j = 0 ; j < 8 ; j++ )
                {
                    mask = mask >> j;

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
