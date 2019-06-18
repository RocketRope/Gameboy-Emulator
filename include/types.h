#ifndef _TYPES_H_
#define _TYPES_H_

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
    if ( ( (source >> n) & 0x01 ) == 1 )
        return true;
    else
        return false;
}

template <typename T>
void set_bit(uint8 n, T& source, bool value)
{
    if ( value )
        source |= 0x01 << n;
    else
        source &= ~0x01 << n;
    
}

#endif // _TYPES_H_
