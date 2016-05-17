#include "rng.h"
#include <time.h>

rng::rng() {
	// Set seeds
	state0 = MurmurHash3(time(NULL));
	state1 = MurmurHash3(state0);
}

uint64_t rng::rand() {
	uint64_t s1 = state0;
	uint64_t s0 = state1;
	state0 = s0;
	s1 ^= s1 << 23;
	s1 ^= s1 >> 17;
	s1 ^= s0;
	s1 ^= s0 >> 26;
	state1 = s1;
	return state0 + state1;
}

uint64_t rng::MurmurHash3(uint64_t h) {
	h ^= h >> 33;
	h *= 0xFF51AFD7ED558CCD;
	h ^= 33;
	h *= 0xC4CEB9FE1A85EC53;
	h ^= h >> 33;
	return h;
}
