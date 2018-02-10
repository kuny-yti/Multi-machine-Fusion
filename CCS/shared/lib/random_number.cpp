#include "random_number.h"
#include <stdio.h>
#include <time.h>

#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long x[N];      /* the array for the state vector  */
static unsigned long *p0, *p1, *pm;

random_number::random_number()
{
    unsigned long init[4] = {0}, length = 4;
    init[0] = (int)time(0);
    init[1] = (int)time(0);
    init[2] = (int)time(0);
    init[3] = (int)time(0);
    init_by_array(init, length);
}

void
random_number::init_genrand(unsigned long s)
{
  int i;

  x[0] = s & 0xffffffffUL;
  for (i = 1; i < N; ++i) {
    x[i] = (1812433253UL * (x[i - 1] ^ (x[i - 1] >> 30)) + i)
      & 0xffffffffUL;           /* for >32 bit machines */
  }
  p0 = x;
  p1 = x + 1;
  pm = x + M;
}

void
random_number::init_by_array(unsigned long init_key[], int key_length)
{
  int i, j, k;

  init_genrand(19650218UL);
  i = 1;
  j = 0;
  for (k = (N > key_length ? N : key_length); k; --k) {
    /* non linear */
    x[i] = ((x[i] ^ ((x[i - 1] ^ (x[i - 1] >> 30)) * 1664525UL))
            + init_key[j] + j) & 0xffffffffUL;  /* for WORDSIZE > 32 machines */
    if (++i >= N) {
      x[0] = x[N - 1];
      i = 1;
    }
    if (++j >= key_length) {
      j = 0;
    }
  }
  for (k = N - 1; k; --k) {
    /* non linear */
    x[i] = ((x[i] ^ ((x[i - 1] ^ (x[i - 1] >> 30)) * 1566083941UL)) - i)
      & 0xffffffffUL;           /* for WORDSIZE > 32 machines */
    if (++i >= N) {
      x[0] = x[N - 1];
      i = 1;
    }
  }
  x[0] = 0x80000000UL;          /* MSB is 1; assuring non-zero initial array */
}

unsigned long
random_number::genrand_int32(void)
{
  unsigned long y;

  if (!p0) {
    /* Default seed */
    init_genrand(5489UL);
  }
  /* Twisted feedback */
  y = *p0 = *pm++ ^ (((*p0 & UPPER_MASK) | (*p1 & LOWER_MASK)) >> 1)
    ^ (-(*p1 & 1) & MATRIX_A);
  p0 = p1++;
  if (pm == x + N) {
    pm = x;
  }
  if (p1 == x + N) {
    p1 = x;
  }
  /* Temper */
  y ^= y >> 11;
  y ^= y << 7 & 0x9d2c5680UL;
  y ^= y << 15 & 0xefc60000UL;
  y ^= y >> 18;
  return y;
}

long
random_number::genrand_int31(void)
{
  return (long) (genrand_int32() >> 1);
}

double
random_number::genrand_real1(void)
{
  return genrand_int32() * (1.0 / 4294967295.0);
  /* divided by 2^32-1 */
}

double
random_number::genrand_real2(void)
{
  return genrand_int32() * (1.0 / 4294967296.0);
  /* divided by 2^32 */
}

double
random_number::genrand_real3(void)
{
  return (((double) genrand_int32()) + 0.5) * (1.0 / 4294967296.0);
  /* divided by 2^32 */
}

double
random_number::genrand_res53(void)
{
  unsigned long a = genrand_int32() >> 5, b = genrand_int32() >> 6;

  return (a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
}

quint64 random_number::genrand_int64()
{
    return genrand_res53()*0xffffffffffffffff;
}

QString random_number::genrand_str(const int len)
{
    char *buff = new char[len+1];
    for(int i = 0; i < len; ++i)
    {
        char a = genrand_int32() % 128;
        if (a < 32)
        {
            buff[i]= 32 + a;
        }
        else
        {
            buff[i]= a;
        }
    }
    buff[len] = '\0';
    QString out = QString(buff);
    delete buff;
    return out;
}
