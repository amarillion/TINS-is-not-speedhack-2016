#include <stdint.h>

#pragma once

#ifndef RNG_H
#define RNG_H

class rng
{
public:
	rng();
	uint64_t rand();
private:
	uint64_t state0 = 1;
	uint64_t state1 = 2;

	uint64_t MurmurHash3(uint64_t h);
};

#endif // RNG_H