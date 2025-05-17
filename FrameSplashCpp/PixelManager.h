#ifndef PIXEL_MANAGER
#define PIXEL_MANAGER
#include "Helper.h"
#include <cassert>
#include <iostream>

class PixelManager {
public:
	void initFilledNeighbours(Pixel *px, Pixel* pixelList, int size);
	void initEmptyNeighbours(Pixel *px, Pixel* pixelList, int size);
	void moveNeighbourToFilled(Pixel *px, Pixel *filledPx);

	void updateAllNeighbours(Pixel* px);

	int isInVector(std::vector<Pixel*> *list, Pixel* px);
};

#endif