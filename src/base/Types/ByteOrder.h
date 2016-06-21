
#ifndef __BYTEORDER_H__
#define __BYTEORDER_H__

#include <stdint.h>
#include "base/Environment.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
/// Get CPU endian type
////////////////////////////////////////////////////////////////////////////////

typedef enum _endian
{
    CPU_LITTLE_ENDIAN,
    CPU_BIG_ENDIAN,
} EndianType;


inline EndianType CheckCPUEndian(void)
{
    uint16_t    x;
    uint8_t     c;
    EndianType  CPUEndian;

    x = 0x0001;;
    c = *(unsigned char *)(&x);

    if( c == 0x01 )
        CPUEndian = CPU_LITTLE_ENDIAN;
    else
        CPUEndian = CPU_BIG_ENDIAN;

    return CPUEndian;
}


////////////////////////////////////////////////////////////////////////////////
/// Basic byte order conversion function
////////////////////////////////////////////////////////////////////////////////

#if PIL_ARCH_LITTLE_ENDIAN   ///< little endian

inline void convByteOrder_h2n_16(void *s, void *d)
{
    uint8_t *_s = (uint8_t*) s,
            *_d = (uint8_t*) d;

    _d[0] = _s[1];
    _d[1] = _s[0];
}

inline void convByteOrder_n2h_16(void *s, void *d)
{
    convByteOrder_h2n_16(s, d);
}

inline void convByteOrder_h2n_32(void *s, void *d)
{
    uint8_t *_s = (uint8_t*) s,
            *_d = (uint8_t*) d;

    _d[0] = _s[3];
    _d[1] = _s[2];
    _d[2] = _s[1];
    _d[3] = _s[0];
}

inline void convByteOrder_n2h_32(void *s, void *d)
{
    convByteOrder_h2n_32(s, d);
}

inline void convByteOrder_h2n_64(void *s, void *d)
{
    uint8_t *_s = (uint8_t*) s,
            *_d = (uint8_t*) d;

    for(int i=0; i<8; i++) _d[i] = _s[7-i];
}

inline void convByteOrder_n2h_64(void *s, void *d)
{
    convByteOrder_h2n_64(s, d);
}

#else                                           ///< big endian

inline void convByteOrder_h2n_16(void *s, void *d)
{
    uint8_t *_s = (uint8_t*) s,
            *_d = (uint8_t*) d;

    _d[0] = _s[0];
    _d[1] = _s[1];
}

inline void convByteOrder_n2h_16(void *s, void *d)
{
    convByteOrder_h2n_16(s, d);
}

inline void convByteOrder_h2n_32(void *s, void *d)
{
    uint8_t *_s = (uint8_t*) s,
            *_d = (uint8_t*) d;

    _d[0] = _s[0];
    _d[1] = _s[1];
    _d[2] = _s[2];
    _d[3] = _s[3];
}

inline void convByteOrder_n2h_32(void *s, void *d)
{
    convByteOrder_h2n_32(s, d);
}

inline void convByteOrder_h2n_64(void *s, void *d)
{
    uint8_t *_s = (uint8_t*) s,
            *_d = (uint8_t*) d;

    for(int i=0; i<8; i++) _d[i] = _s[i];
}

inline void convByteOrder_n2h_64(void *s, void *d)
{
    convByteOrder_h2n_64(s, d);
}

#endif




////////////////////////////////////////////////////////////////////////////////
/// Automatic byte order conversion function
////////////////////////////////////////////////////////////////////////////////

template<class T>
inline T convByteOrder_h2n(T s)
{
    if( sizeof(T) == 2 ) {
        uint16_t *_s, *_d;
        T        v;

        _s = (uint16_t*) &s;
        _d = (uint16_t*) &v;

        convByteOrder_h2n_16(_s, _d);
        return v;
    } else if( sizeof(T) == 4 ) {
        uint32_t *_s, *_d;
        T        v;

        _s = (uint32_t*) &s;
        _d = (uint32_t*) &v;

        convByteOrder_h2n_32(_s, _d);
        return v;
    } else if( sizeof(T) == 8 ) {
        uint64_t *_s, *_d;
        T        v;

        _s = (uint64_t*) &s;
        _d = (uint64_t*) &v;

        convByteOrder_h2n_64(_s, _d);
        return v;
    }
}

template<class T>
inline T convByteOrder_n2h(T s)
{
    if( sizeof(T) == 2 ) {
        uint16_t *_s, *_d;
        T        v;

        _s = (uint16_t*) &s;
        _d = (uint16_t*) &v;

        convByteOrder_n2h_16(_s, _d);
        return v;
    } else if( sizeof(T) == 4 ) {
        uint32_t *_s, *_d;
        T        v;

        _s = (uint32_t*) &s;
        _d = (uint32_t*) &v;

        convByteOrder_n2h_32(_s, _d);
        return v;
    } else if( sizeof(T) == 8 ) {
        uint64_t *_s, *_d;
        T        v;

        _s = (uint64_t*) &s;
        _d = (uint64_t*) &v;

        convByteOrder_n2h_64(_s, _d);
        return v;
    }
}

} // end of namespace pi

#endif // end of __BYTEORDER_H__
