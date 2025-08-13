#ifndef RENDER_ENGINE
#define RENDER_ENGINE
#include <iostream>
#include <string>
#include <cassert>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <execution>  // For std::execution::par
#include <atomic>
#include <limits> // for std::numeric_limits

#include "PixelManager.h"
#include "PerlinReader.h"

class RenderEngine {
private:
	//Neighbour offsets
	const int dx[8] = { 0,  1,  1,  1,  0, -1, -1, -1 };
	const int dy[8] = { -1, -1,  0,  1,  1,  1,  0, -1 };

	//Neighbour inverses
	const int inverseIndex[8] = {4, 5, 6, 7, 0, 1, 2, 3};
	const int ix[8] = {0, -1, -1, -1, 0, 1, 1, 1};
	const int iy[8] = {1, 1, 0, -1, -1, -1, 0, 1};

	Colour* colourStack;
	Pixel* fullGrid;
	PixelDentch* fullDentchGrid;
	std::vector<PixelDentch*> dentchAvailArray;

	std::vector<Pixel*> availArray;

	PixelManager pxMgr;
	PerlinReader perlinReader;
	bool PERLIN = true;
	bool TEMPLATE = true;
	bool AVERAGE = false;
	bool PARALLEL = true;

	bool STACKING_RESLUTION = false;
	const int STACK_LAYERS = 4;
	int stackLayers = 0;

	int HEIGHT;
	int WIDTH;
	
	int start_x;
	int start_y;
	
	int availableSize = 0;

	void initPixel(Pixel *px);

	void initializeDentchGrid();

	void addToAvailArray(Pixel* px);
	void addToAvailArrayDentch(PixelDentch* px);

	void removeFromAvailArray(Pixel* px);
	void removeFromAvailArrayDentch(PixelDentch* px);

	void addNeighboursToAvailArray(Pixel* px);
	void addNeighboursToAvailArray(PixelDentch* px);
		
	void printColour(Colour c, std::string msg = "");
	void printPixel(Pixel px, std::string msg = "");
	void printAvailable(std::string msg = "");
	void printMemoryAddress(Pixel *px, std::string msg = "");

	PixelDentch* parallel_AvgMinDiff(Colour toCompare, int i);
	PixelDentch* parallel_MinDiff(Colour toCompare, int i);

	Pixel* findMinDiff(Colour toCompare, int i);
	PixelDentch* findMinDiffDentch(Colour toCompare, int i);
	PixelDentch* findAverageMinDiffDentch(Colour toCompare, int i);

	PixelDentch* bestByPerlin(PixelDentch* px, Colour toCompare);

	Pixel indexToXY(int index);
	int colourDifference(Colour c1, Colour c2);

	std::vector<PixelDentch*> GetCirclePoints(int centreX, int centreY, int radius);
	std::vector<PixelDentch*> GetCanvasOutlinePoints();

public:
	void init(int h, int w, Colour* stack, std::string inputType = "rgb");
	void initStackedCanvas(int multiplier);
	void runMainLoop(int resolutionOffset = 0);
	void renderFrame(int layer, int count);
};

#endif RENDER_ENGINE