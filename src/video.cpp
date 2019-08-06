#include "video.h"

#include <map>

// Constructor //

PPU::PPU(MCU* _mcu) :
    mcu(_mcu),
    stat ( mcu->get_memory_reference(MCU::ADDRESS::STAT) ),
    lyc_match_interrupt(stat), 
    oam_search_interrupt(stat), 
    vblank_interrupt(stat), 
    hblank_interrupt(stat),
    lyc_match_flag(stat),
    lcdc ( mcu->get_memory_reference(MCU::ADDRESS::LCDC)  ),
    lcd_enabled(lcdc),
    window_tile_map(lcdc),
    window_enabled(lcdc),
    tile_data_map(lcdc),
    bg_tile_map(lcdc),
    sprite_size(lcdc),
    sprite_enabled(lcdc),
    bg_enabled(lcdc),
    ly   ( mcu->get_memory_reference(MCU::ADDRESS::LY)   ),
    lyc  ( mcu->get_memory_reference(MCU::ADDRESS::LYC)  ),
    scx  ( mcu->get_memory_reference(MCU::ADDRESS::SCX)  ),
    scy  ( mcu->get_memory_reference(MCU::ADDRESS::SCY)  ),
    wx   ( mcu->get_memory_reference(MCU::ADDRESS::WX)   ),
    wy   ( mcu->get_memory_reference(MCU::ADDRESS::WY)   ),
    if_vblank_bit( mcu->get_memory_reference(MCU::ADDRESS::IF) ),
    bgp  ( mcu->get_memory_reference(MCU::ADDRESS::BGP)  ),
    obp0 ( mcu->get_memory_reference(MCU::ADDRESS::OBP0) ),
    obp1 ( mcu->get_memory_reference(MCU::ADDRESS::OBP1) )
    
{
    reset();
}

//

void PPU::reset()
{
    scanline_vertical_counter = 0;

    set_mode( MODE::OAM_SEARCH );

    clear_framebuffer();
}

//

void PPU::step(uint16 elapsed_clocks)
{
    scanline_vertical_counter += elapsed_clocks;

    if ( scanline_vertical_counter > 456 )
    {
        // Next scanline
        scanline_vertical_counter -= 456;
        ly++;

        if ( ly == lyc )
            lyc_match_flag = true;
        else
            lyc_match_flag = false;

        draw_scanline();
    }

    if ( ly < 144 )
    {
        // Set mode
        if ( scanline_vertical_counter < 80 )
            set_mode( MODE::OAM_SEARCH );
        else if ( scanline_vertical_counter < 248 )
            set_mode( MODE::VRAM_TRANSFER );
        else
            set_mode( MODE::H_BLANK );
    }
    else // V BLANK
    {
        // Start new frame when ly > 153
        if ( ly > 153 )
        {
            ly = 0;

            // Request interrupt
            if_vblank_bit = true;

            if ( vblank_callback )
                vblank_callback();

            clear_framebuffer();
        }

        set_mode( MODE::V_BLANK );
    }
}

//

void PPU::get_framebuffer(RGB_Framebuffer rgb_framebuffer, RGB_Palette palette)
{
    for ( int y = 0 ; y < screen_height ; y++ )
        for ( int x = 0 ; x  < screen_width ; x++ )
            rgb_framebuffer[y][x] = palette[ framebuffer[y][x] ];
}

// Callback register function //

void PPU::register_vblank_callback(std::function<void()> callback)
{
    vblank_callback = callback;
}

//

void PPU::set_mode(MODE mode)
{
    current_mode = mode;

    stat = (stat & 0x7C) | (mode & 0x03);
}

//

void PPU::clear_framebuffer()
{
    for ( int y = 0 ; y < screen_height ; y++ )
        for ( int x = 0 ; x < screen_width ; x++ )
            framebuffer[y][x] = 0x00;
}

void PPU::draw_scanline()
{
    if ( ly < 144 )
    {
        draw_scanline_bg_pixels();
        draw_scanline_sprite_pixels();
    }
}

void PPU::draw_scanline_bg_pixels()
{
    uint8 offset_y = ly + scy;

    for( uint8 x = 0 ; x < screen_width ; x++ )
    {
        uint8 offset_x = x + scx;

        // Find bg tile
        uint16 bg_index   = (offset_x / 8) + ( (offset_y / 8) * 32 );
        uint8  tile_index = mcu->bg_map_0[bg_index];
        Tile   tile       = mcu->tile_map_0[tile_index];

        framebuffer[ly][x] = tile.get_pixel(offset_x % 8, offset_y % 8);
    }
}

void PPU::draw_scanline_sprite_pixels()
{

}

std::vector<const Sprite*> PPU::oam_search()
{
    std::vector<const Sprite*> sprites;

    for ( int i = 0 ; i < 40 ; i++ )
    {
        Sprite* sprite = &mcu->objects[i];

        if ( ( sprite->pos_y >= ly ) && ( sprite->pos_y <= (ly + 8) ) )
            sprites.push_back(sprite);
    }

    std::sort( sprites.begin(), sprites.end(), []( const Sprite* a,const Sprite* b){ return a->pos_x > b->pos_y; } );

    return sprites;
}

const Sprite* PPU::get_highest_priority_sprite(uint8 x, const std::vector<const Sprite*>& sprites)
{
    for ( const Sprite* sprite : sprites )
    {
        if ( ( sprite->pos_x >= x ) && (sprite->pos_x <= (x + 8) ) )
            return sprite;

        if ( sprite->pos_x > (x + 8) )
            return nullptr;
    }

    return nullptr;
}



