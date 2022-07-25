#include "noise.h"

uint32_t
noise(int position, uint32_t seed)
{
    const uint32_t NOISE1 = 0xB5297A4D;
    const uint32_t NOISE2 = 0x68E31DA4;
    const uint32_t NOISE3 = 0x1B56C4E9;

    uint32_t mangled = position;
    mangled *= NOISE1;
    mangled += seed;
    mangled ^= (mangled >> 8);
    mangled += NOISE2;
    mangled ^= (mangled << 8);
    mangled *= NOISE3;
    mangled ^= (mangled >> 8);
    return mangled;
}
