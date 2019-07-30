#include "types.h"

const Color Color::WHITE      = 0xFFFFFF;
const Color Color::LIGHT_GREY = 0xC0C0C0;
const Color Color::GREY       = 0x7F7F7F;
const Color Color::DARK_GREY  = 0x404040;
const Color Color::BLACK      = 0x000000;

const Color Color::GB_WHITE      = 0x9BBC0F;
const Color Color::GB_LIGHT_GREY = 0x8BAC0F;
const Color Color::GB_DARK_GREY  = 0x306230;
const Color Color::GB_BLACK      = 0x0F380F;

const RGB_Palette RGB_Palette::BLACK_AND_WHITE{Color::WHITE, Color::LIGHT_GREY, Color::DARK_GREY, Color::BLACK};
const RGB_Palette RGB_Palette::GB_ORIGINAL_GREEN{Color::GB_WHITE, Color::GB_LIGHT_GREY, Color::GB_DARK_GREY, Color::GB_BLACK};
