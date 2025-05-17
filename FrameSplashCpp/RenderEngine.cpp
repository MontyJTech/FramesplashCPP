#include "RenderEngine.h"


void RenderEngine::init(int h, int w, Colour* stack, std::string inputType) {
	HEIGHT = h;
	WIDTH = w;
	int size = h * w;
	availArray = std::vector<Pixel*>();
	dentchAvailArray = std::vector<PixelDentch*>();

	fullDentchGrid = new PixelDentch[size];
	initializeDentchGrid();

	pxMgr = PixelManager();
	perlinReader = PerlinReader();
	colourStack = stack;

	if (PERLIN) { perlinReader.generate(WIDTH, HEIGHT); }

	start_x = WIDTH/2;
	start_y = HEIGHT/2;
	//if (TEMPLATE) {
	//	templateEngine = TemplateEngine();
	//	templateEngine.openWindow();
	//}
	//cuda_init(w, h, pixelGrid);
}

int RenderEngine::colourDifference(Colour c1, Colour c2) {
	int R = c1.r - c2.r;
	int G = c1.g - c2.g;
	int B = c1.b - c2.b;

	return (R * R) + (G * G) + (B * B);
}

void RenderEngine::initPixel(Pixel *px) {
	for (int dy = -1; dy <= 1; dy++)
	{
		if (px->y + dy != -1 && px->y + dy != HEIGHT) 
			for (int dx = -1; dx <= 1; dx++)
			{
				if (px->x + dx != -1 && px->x + dx != WIDTH) {
					if (!(dx == 0 && dy == 0)) {
						int index = (px->x + dx) + ((px->y + dy) * WIDTH);
						if (fullGrid[index].isEmpty) {
							fullGrid[index].x = px->x + dx;
							fullGrid[index].y = px->y + dy;

							px->emptyPixelList.push_back(&fullGrid[index]);
						}
						else {
							px->filledPixelList.push_back(&fullGrid[index]);
						}
					}
				}
			}
	}
}

void RenderEngine::initializeDentchGrid() {
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			int index = x + (y * WIDTH);

			fullDentchGrid[index].x = x;
			fullDentchGrid[index].y = y;
		}
	}
}

Pixel RenderEngine::indexToXY(int index) {
	Pixel values;
	int y = index / WIDTH;
	int x = index % WIDTH;
	values = Pixel(x, y);
	return values;
};

Pixel* RenderEngine::findMinDiff(Colour toCompare, int i) {
	if (i == 0) {
		Pixel* tempRef = &fullGrid[start_x + (start_y * WIDTH)];
		tempRef->x = start_x;
		tempRef->y = start_y;
		addToAvailArray(tempRef);
		return tempRef;
	}
	else {
		int curMinValue = -1;
		int curMinPtr = 0;
		for (Pixel* elem : availArray) {
			for (Pixel* nPX : elem->filledPixelList) {
				int colourDiff = colourDifference(nPX->col, toCompare);
				if (colourDiff < curMinValue || curMinValue < 0) {
					curMinValue = colourDiff;
					curMinPtr = elem->index;
				}
			}
		}
		
		return availArray[curMinPtr];
	}
}

PixelDentch* RenderEngine::findMinDiffDentch(Colour toCompare, int i) {
	if (i == 0) {
		PixelDentch* tempRef = &fullDentchGrid[start_x + (start_y * WIDTH)];
		addToAvailArrayDentch(tempRef);

		return tempRef;
	}
	else {
		int curMinValue = -1;
		int curMinPtr = 0;
		for (PixelDentch* wham : dentchAvailArray) {
			for (int i = 0; i < 8; i++) {
				if (wham->neighbors[i] != 0) //if neighbour is empty, continue
				{
					int nx = wham->x + dx[i];
					int ny = wham->y + dy[i];

					if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
						int neighborIndex = nx + ny * WIDTH;
						int colourDiff = colourDifference(fullDentchGrid[neighborIndex].col, toCompare);
						if (colourDiff < curMinValue || curMinValue < 0) {
							curMinValue = colourDiff;
							curMinPtr = wham->index;
						}
					}
				}
			}
		}

		return dentchAvailArray[curMinPtr];
	}
}

PixelDentch* RenderEngine::findAverageMinDiffDentch(Colour toCompare, int i) {
	if (i == 0) {
		PixelDentch* tempRef = &fullDentchGrid[start_x + (start_y * WIDTH)];
		addToAvailArrayDentch(tempRef);

		return tempRef;
	}
	else {
		int curMinValue = -1;
		int curMinPtr = 0;
		int colourDiff = 0;

		for (PixelDentch* wham : dentchAvailArray) {
			int count = 0;
			Colour avgColour;
			colourDiff = 0;
			for (int i = 0; i < 8; i++) {
				if (wham->neighbors[i] != 0) //if neightbour is empty, continue
				{
					int nx = wham->x + dx[i];
					int ny = wham->y + dy[i];

					if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
						int neighborIndex = nx + ny * WIDTH;
						Colour pxColour = fullDentchGrid[neighborIndex].col;
						avgColour.r += pxColour.r;
						avgColour.g += pxColour.g;
						avgColour.b += pxColour.b;
					}
					count++;
				}
				if (count > 0) {
					avgColour.r /= count;
					avgColour.g /= count;
					avgColour.b /= count;
				}

				colourDiff = colourDifference(avgColour, toCompare);
				if (colourDiff < curMinValue || curMinValue < 0) {
					curMinValue = colourDiff;
					curMinPtr = wham->index;
				}
				if (colourDiff == 0) {
					return dentchAvailArray[curMinPtr];
				}
			}
		}

		return dentchAvailArray[curMinPtr];
	}
}

//12 minutes 5 seconds 1000x1000
PixelDentch* RenderEngine::parallel_AvgMinDiff(Colour toCompare, int i) {
	if (i == 0) {
		PixelDentch* tempRef = &fullDentchGrid[start_x + (start_y * WIDTH)];
		addToAvailArrayDentch(tempRef);

		return tempRef;
	}

	std::atomic<int> curAtomicMinValue{ std::numeric_limits<int>::max() };
	std::atomic<int> curAtomicMinPtr{ 0 };

	std::for_each(std::execution::par, dentchAvailArray.begin(), dentchAvailArray.end(),
		[&](PixelDentch* wham) {
			for (int i = 0; i < 8; i++) {
				if (wham->neighbors[i] != 0) {
					int nx = wham->x + dx[i];
					int ny = wham->y + dy[i];

					if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
						int neighborIndex = nx + ny * WIDTH;
						int colourDiff = colourDifference(fullDentchGrid[neighborIndex].col, toCompare);

						int curVal = curAtomicMinValue.load(std::memory_order_relaxed);
						while (colourDiff < curVal && !curAtomicMinValue.compare_exchange_weak(curVal, colourDiff, std::memory_order_relaxed)) {
							// retry if another thread already updated with a closer match
						}

						if (colourDiff == curAtomicMinValue.load(std::memory_order_relaxed)) {
							curAtomicMinPtr.store(wham->index, std::memory_order_relaxed);
						}
					}
				}
			}
		});

	return dentchAvailArray[curAtomicMinPtr];
}

PixelDentch* RenderEngine::bestByPerlin(PixelDentch* px, Colour toCompare) {
	std::pair<int, int> perlinOffset = perlinReader.getOffsetCoords(px->x, px->y);
	for (int i = 0; i < 8; i++) {
		if (px->neighbors[i] == 0) {
			int nx = px->x + dx[i];
			int ny = px->y + dy[i];
			int neighborIndex = nx + ny * WIDTH;

			if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && dx[i] == perlinOffset.first && dy[i] == perlinOffset.second && fullDentchGrid[neighborIndex].isEmpty && fullDentchGrid[neighborIndex].isAvailable)
			{
				return &fullDentchGrid[neighborIndex];
			}
		}
	}
	return px;
}
void RenderEngine::renderFrame(int count) {
	cv::Mat newFrame = cv::Mat(WIDTH, HEIGHT, CV_8UC3);
	for (int i = 0; i < HEIGHT * WIDTH; i++) {
		if (!fullDentchGrid[i].isEmpty) {
			//Pixel coords = indexToXY(i);
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].x, fullDentchGrid[i].y)[0] = fullDentchGrid[i].col.b;
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].x, fullDentchGrid[i].y)[1] = fullDentchGrid[i].col.g;
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].x, fullDentchGrid[i].y)[2] = fullDentchGrid[i].col.r;
		}
	}
	cv::imwrite("D:\\CodingProjects\\FrameSplashCpp\\FrameSplashCpp\\x64\\Debug\\render\\Output\\frame" + std::to_string(count) + ".jpg", newFrame);
}

void RenderEngine::addToAvailArray(Pixel* px) {
	initPixel(px);
	availArray.push_back(px);
	px->index = availArray.size() - 1;
	px->isAvailable = true;
}

void RenderEngine::addToAvailArrayDentch(PixelDentch* px) {
	dentchAvailArray.push_back(px);
	px->index = dentchAvailArray.size() - 1;
	px->isAvailable = true;
}

void RenderEngine::removeFromAvailArray(Pixel* px) {
	auto it = std::find(availArray.begin(), availArray.end(), px);
	int index = it - availArray.begin();
	if (it == availArray.end()) 
		index = -1;
	
	assert(index > -1);
	availArray.erase(availArray.begin() + index);
	for (int i = index; i < availArray.size(); i++) {
		availArray.at(i)->index--;
	}
}

void RenderEngine::removeFromAvailArrayDentch(PixelDentch* px) {
	auto it = std::find(dentchAvailArray.begin(), dentchAvailArray.end(), px);
	int index = it - dentchAvailArray.begin();
	if (it == dentchAvailArray.end())
		index = -1;

	assert(index > -1);
	dentchAvailArray.erase(dentchAvailArray.begin() + index);
	for (int i = index; i < dentchAvailArray.size(); i++) {
		dentchAvailArray.at(i)->index--;
	}
}


void RenderEngine::addNeighboursToAvailArray(Pixel* px) {
	for (Pixel* elem : px->emptyPixelList) {
		if (!elem->isAvailable) {
			addToAvailArray(elem);
		}
		else {
			pxMgr.moveNeighbourToFilled(elem, px);
		}
	}
	for (Pixel* elem : px->filledPixelList) {
		pxMgr.moveNeighbourToFilled(elem, px);
	}
}

void RenderEngine::addNeighboursToAvailArray(PixelDentch* px) {

	for (int i = 0; i < 8; i++) {
		int nx = px->x + dx[i];
		int ny = px->y + dy[i];

		if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) 
		{
			int neighborIndex = nx + ny * WIDTH;
			if (fullDentchGrid[neighborIndex].isEmpty) 
			{
				if (!fullDentchGrid[neighborIndex].isAvailable) 
				{
					addToAvailArrayDentch(&fullDentchGrid[neighborIndex]);
				}
				else 
				{
					fullDentchGrid[neighborIndex].neighbors[inverseIndex[i]] = 1;
				}
			}
			else 
			{
				fullDentchGrid[neighborIndex].neighbors[inverseIndex[i]] = 1;
			}
		}
	}
}

void RenderEngine::printColour(Colour c, std::string msg) {
	std::cout << msg << " - Colour: " << c.r << ", " << c.g << ", " << c.b << std::endl;
}

void RenderEngine::printPixel(Pixel px, std::string msg) {
	std::cout << msg << " - Pixel: " << px.x << ", " << px.y << " Index: " << px.index << std::endl;
}
//void RenderEngine::printAvailable(std::string msg) {
//	std::cout << "Cur available pixels: " << std::endl;
//	for(int i = 0; i < availableSize; i++){
//		printPixel(*available[i]);
//	}
//}
void RenderEngine::printMemoryAddress(Pixel *px, std::string msg) {
	std::cout << msg << " Memory Address: " << px << std::endl;
}

void RenderEngine::runMainLoop() {
	int checkpoint = 1.f * ((HEIGHT * WIDTH) / 100); // percentage * totalsize/100
	int frameCounter = 0; 
	std::cout << "Running main loop..." << std::endl;
	for (int i = 0; i < HEIGHT * WIDTH; i++) {
		if (TEMPLATE) {

		}
		PixelDentch* curPXD;

		if (PARALLEL) {
			curPXD = parallel_AvgMinDiff(colourStack[i], i);
		}
		else if (AVERAGE) {
			curPXD = findAverageMinDiffDentch(colourStack[i], i);
		}
		else {
			curPXD = findMinDiffDentch(colourStack[i], i);
		}

		if (PERLIN && i != 0) {
			curPXD = bestByPerlin(curPXD, colourStack[i]);
		}

		if (i % 256 == 0)
			std::cout << std::to_string((double)i / WIDTH / HEIGHT) << " stack: " <<  dentchAvailArray.size() << std::endl;
		int index = curPXD->x + (curPXD->y * WIDTH);

		assert(index >= 0 && index < WIDTH * HEIGHT);
  		assert(fullDentchGrid[index].isEmpty);

		fullDentchGrid[index].col = colourStack[i];
		fullDentchGrid[index].isEmpty = false;

		removeFromAvailArrayDentch(curPXD);
		addNeighboursToAvailArray(curPXD);

		//if (i > 0) { pxMgr.updateAllNeighbours(curPXD); }

		if (i % checkpoint == 0 && i > 0) {
			renderFrame(frameCounter++);
		}
	}
	renderFrame(frameCounter++);
}
