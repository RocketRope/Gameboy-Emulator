#include "types.h"

// 24bit RGB Color const declarations //

const RGB_Color RGB_Color::WHITE      = 0xFFFFFF;
const RGB_Color RGB_Color::LIGHT_GREY = 0xC0C0C0;
const RGB_Color RGB_Color::GREY       = 0x7F7F7F;
const RGB_Color RGB_Color::DARK_GREY  = 0x404040;
const RGB_Color RGB_Color::BLACK      = 0x000000;

const RGB_Color RGB_Color::GB_WHITE      = 0x9BBC0F;
const RGB_Color RGB_Color::GB_LIGHT_GREY = 0x8BAC0F;
const RGB_Color RGB_Color::GB_DARK_GREY  = 0x306230;
const RGB_Color RGB_Color::GB_BLACK      = 0x0F380F;

// 24bit RGB Palette const declarations //

const RGB_Palette RGB_Palette::BLACK_AND_WHITE{ RGB_Color::WHITE,
                                                RGB_Color::LIGHT_GREY,
                                                RGB_Color::DARK_GREY,
                                                RGB_Color::BLACK };

const RGB_Palette RGB_Palette::GB_ORIGINAL_GREEN{ RGB_Color::GB_WHITE,
                                                  RGB_Color::GB_LIGHT_GREY,
                                                  RGB_Color::GB_DARK_GREY,
                                                  RGB_Color::GB_BLACK };

// Tile helper class //

uint8 Tile::get_pixel(uint8 x, uint8 y)
{
    if ( y > 7 )
        return 0x00;

    uint8 pixel_value = 0;
    uint8 mask = 0x80 >> x;

    uint8 index = y * 2;

    if ( data[index] & mask )
        pixel_value += 1;
    if ( data[index + 1] & mask )
        pixel_value += 2;

    return pixel_value;
}

void Tile::to_rgb( Tile_Pixel_Array pixels, uint8 gb_palette, const RGB_Palette& rgb_palette ) const
{
    size_t index = 0;

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

            pixels[index] = rgb_palette[pixel_color];

            index++;
        }
    }
}

// Sprite helper class

const std::function<bool(const Sprite, const Sprite)> Sprite::comp_x = [](const Sprite& a, const Sprite& b) { return a.pos_x > b.pos_x; };
const std::function<bool(const Sprite, const Sprite)> Sprite::comp_y = [](const Sprite& a, const Sprite& b) { return a.pos_y > b.pos_y; };
