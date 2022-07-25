#ifndef NOISE_H
#define NOISE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int position;
    uint32_t seed;
} RNG;

uint32_t noise(int position, uint32_t seed);
#define RNG_NEXT(rng_pointer) noise((rng->position)++, rng->seed)
#define RNG_1f(rng_pointer) ((float)RNG_NEXT(rng_pointer) / (float)UINT32_MAX)
#define RNG_INDEX(rng_pointer, max_index) ((size_t)(RNG_1f(rng_pointer) * (max_index + 1)))

#endif // NOISE_H
