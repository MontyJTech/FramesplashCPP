#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS
#include <vector>
#include <bitset>
#include <cstdint>

struct Colour {
	int r;
	int g;
	int b;

	Colour(int r, int g, int b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	Colour() {
		this->r = 0;
		this->g = 0;
		this->b = 0;
	}
};

struct Pixel {
	int x, y;
	int index = -1;
	int nAvg = 0;

	std::vector<Pixel*> emptyPixelList;
	std::vector<Pixel*> filledPixelList;

	bool isEmpty = true;
	bool isAvailable = false;
	Colour col = Colour(0,0,0);

	Pixel(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Pixel() {
		this->x = 0;
		this->y = 0;
	}
};

// Assuming 8 neighbors max (N, NE, E, SE, S, SW, W, NW)
constexpr int MAX_NEIGHBORS = 8;

struct PixelDentch {
	uint16_t x, y;           // 2 bytes each, sufficient for 65k x 65k resolution
	int64_t index = -1;      
	uint8_t nAvg = 0;        // Assuming it's a small counter

	// Bitset: 1 for filled, 0 for empty. If you need pointers, store them externally - ordered as (N, NE, E, SE, S, SW, W, NW)
	std::bitset<MAX_NEIGHBORS> neighbors;

	union {
		struct {
			bool isEmpty : 1;
			bool isAvailable : 1;
		};
		uint8_t flags = 0;
	};

	Colour col;

	PixelDentch(uint16_t x, uint16_t y) : x(x), y(y), isEmpty(true), isAvailable(false) {};

	PixelDentch() : x(0), y(0), isEmpty(true), isAvailable(false) {};
};

#endif