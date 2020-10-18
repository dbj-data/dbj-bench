#include "../ubench.h/ubench.h"

static long CountOne1 (long n) {
  long i = 0, j = 1;
  long count = 0;
  for (i = 1; i <= n; i++) {
    j = i;
    while (j != 0) {
      if (j % 10 == 1)
        count++;
      j /= 10; // check rest digits
    }
  }
  return count;
}

static long CountOne2(long n) {
  if (n == 0)
    return 0;
  if (n < 10)
    return 1;
  long count = 0;
  long highest = n;
  int weight = 1;
  while (highest >= 10) {
    highest /= 10;
    weight *= 10;
  }
  if (highest == 1) {
    count = CountOne2(weight - 1) + CountOne2(n - weight) + n - weight + 1;
  } else {
    count = highest * CountOne2(weight - 1) + CountOne2(n - highest * weight) +
            weight;
  }
  return count;
}

static long CountOne3(long n) {
  long count = 0;
  long i = 1;
  long current = 0, after = 0, before = 0;
  while ((n / i) != 0) {
    current = (n / i) % 10;
    before = n / (i * 10);
    after = n - (n / i) * i;
    if (current > 1)
      count = count + (before + 1) * i;
    else if (current == 0)
      count = count + before * i;
    else if (current == 1)
      count = count + before * i + after + 1;
    i *= 10;
  }
  return count;
}

static long f(int n) // counts how many 1's for number n
{
	long out = 0;
	for ( ; n != 0; n /= 10)
		out += (n % 10 == 1);
	return out;
}

static long CountOne4(long n) {
  if (n < 1)
    return 0;
  const long d = n / 10;
  const long u = n % 10;
  if (u == 9)
    return CountOne4(d) * 10 + d + 1;
  return CountOne4(n - (u + 1)) + f(d) * (u + 1) + (u > 0);
}

/********************************************************************/

static struct counting_to { long N; } metadata = {0xFFFFF};

UBENCH(count_ones, 1) { CountOne1(metadata.N); }

UBENCH(count_ones, 2) { CountOne2(metadata.N); }

UBENCH(count_ones, 3) { CountOne3(metadata.N); }

UBENCH(count_ones, 4) { CountOne4(metadata.N); }
