#pragma once
#include <stdint.h>

/*
    Simple replacement for srand() and rand()
    Uses xorshift32 (fast, decent statistical quality for general use)
    NOT cryptographically secure.
*/

#define MY_DEFAULT_SEED 2463534242u
static uint32_t my_rand_state = MY_DEFAULT_SEED;  // default non-zero seed

static inline void my_srand(uint32_t seed)
{
  if (seed == 0)
    seed = MY_DEFAULT_SEED;  // avoid zero state
  my_rand_state = seed;
}

static inline int my_rand(void)
{
  uint32_t x = my_rand_state;

  // xorshift32 algorithm
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;

  my_rand_state = x;

  // Match traditional rand() range: 0 to 0x7fffffff
  return (int)(x & 0x7fffffff);
}