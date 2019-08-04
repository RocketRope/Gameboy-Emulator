#ifndef _VIDEO_H_
#define _VIDEO_H_

#include "types.h"
#include "memory.h"

typedef RGB_Color RGB_Framebuffer[144][160];

class PPU
{
    public:

        enum MODE : uint8
        {
            H_BLANK       = 0x00,
            V_BLANK       = 0x01,
            OAM_SEARCH    = 0x02,
            VRAM_TRANSFER = 0x03
        };

        // Constructor //

        PPU(MCU* _mcu);

        //

        void step(uint16 elapsed_clocks);

        //

        void get_framebuffer( RGB_Framebuffer rgb_framebuffer, 
                              RGB_Palette palette = RGB_Palette::BLACK_AND_WHITE );

        // Callback register function //

        void register_v_blank_callback(std::function<void()> callback);

    private:

        const uint8 screen_width  = 160;
        const uint8 screen_height = 144;

        MCU* mcu;

        uint16 scanline_vertical_counter;

        MODE current_mode;
        uint8 framebuffer[144][160] = {3};

        // IO Register reference
        
        uint8& stat;
        Bit<6> lyc_match_interrupt;
        Bit<5> oam_search_interrupt;
        Bit<4> v_blank_interrupt;
        Bit<3> h_blank_interrupt;
        Bit<2> lyc_match_flag;

        uint8& lcdc;
        Bit<7> lcd_enabled;
        Bit<6> window_tile_map;
        Bit<5> window_enabled;
        Bit<4> tile_data_map;
        Bit<3> bg_tile_map;
        Bit<2> object_size;
        Bit<1> object_enabled;
        Bit<0> bg_enabled;

        uint8& ly;
        uint8& lyc;

        uint8& scx;
        uint8& scy;

        uint8& wx;
        uint8& wy;

        // Palette reference

        uint8& bgp;
        uint8& obp0;
        uint8& obp1;

        // Callback function variable
        std::function<void()> v_blank_callback;

        //
        
        void set_mode(MODE mode);
        void draw_scanline();
};

#endif // _VIDEO_H_
