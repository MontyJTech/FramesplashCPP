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

	if (PERLIN) { perlinReader.generate(HEIGHT, WIDTH); }

	start_x = WIDTH / 2;
	start_y = HEIGHT / 2;
}

void RenderEngine::initStackedCanvas(int multiplier) {
	assert(dentchAvailArray.size() == 0);

	int newWidth = (WIDTH * 2) - 1;
	int newHeight = (HEIGHT * 2) - 1;

	if (PERLIN) { perlinReader.generate(newHeight, newWidth); }

	PixelDentch* newGrid = new PixelDentch[newHeight * newWidth];

	int filledCounter = 0;
	int emptyCounter = 0;

	for (int y = 0; y < newHeight; y++) {
		for (int x = 0; x < newWidth; x++) {
			int index = x + (y * newWidth);

			newGrid[index].x = x;
			newGrid[index].y = y;

			if (x % 2 == 0 && y % 2 == 0) {
				newGrid[index].col = fullDentchGrid[filledCounter].col;
				newGrid[index].isAvailable = false;
				newGrid[index].isEmpty = false;
				filledCounter++;
			}
			else {
				newGrid[index].isEmpty = true;
				emptyCounter++;
			}
		}
	}

	std::cout << "initialized new grid. Filled pixels: " + std::to_string(filledCounter) + ", empty pixels: " + std::to_string(emptyCounter) + ", total pixels: " + std::to_string(emptyCounter + filledCounter) + ". " << std::endl;
	WIDTH = newWidth;
	HEIGHT = newHeight;

	fullDentchGrid = newGrid;

	for (int index = 0; index < newWidth * newHeight; index++) {
		if (fullDentchGrid[index].isEmpty) {
			addToAvailArrayDentch(&fullDentchGrid[index]);
		}

		for (int i = 0; i < 8; i++) {
			int nx = fullDentchGrid[index].x + dx[i];
			int ny = fullDentchGrid[index].y + dy[i];

			if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT)
			{
				int neighborIndex = nx + ny * WIDTH;
				if (fullDentchGrid[neighborIndex].isEmpty)
				{
					fullDentchGrid[index].neighbors[i] = 0;
				}
				else
				{
					fullDentchGrid[index].neighbors[i] = 1;
				}
			}
		}
	}

	cv::Mat newFrame = cv::Mat(HEIGHT, WIDTH, CV_8UC3);
	for (int i = 0; i < HEIGHT * WIDTH; i++) {
		if (!fullDentchGrid[i].isEmpty) {
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].x, fullDentchGrid[i].y)[0] = fullDentchGrid[i].col.b;
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].x, fullDentchGrid[i].y)[1] = fullDentchGrid[i].col.g;
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].x, fullDentchGrid[i].y)[2] = fullDentchGrid[i].col.r;
		}
	}
	cv::imwrite("D:\\CodingProjects\\FrameSplashCpp\\FrameSplashCpp\\x64\\Debug\\render\\Output\\ScaleUpFrame.jpg", newFrame);
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
	if (i == 0 && stackLayers == 0) {
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
	if (i == 0 && stackLayers == 0) {
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
			Colour nColours[8];

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
						nColours[count] = pxColour;
						avgColour.r += pxColour.r;
						avgColour.g += pxColour.g;
						avgColour.b += pxColour.b;

						count++;
					}
				}
			}
			if (count == 0) { count = 1; }
			if (count == 4) {
				avgColour.r = nColours[0].r + nColours[1].r / 2;
				avgColour.g = nColours[0].g + nColours[1].g / 2;
				avgColour.b = nColours[0].b + nColours[1].b / 2;
			}
			else{
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

		return dentchAvailArray[curMinPtr];
	}
}

PixelDentch* RenderEngine::parallel_AvgMinDiff(Colour toCompare, int index) {
	if (index == 0 && stackLayers == 0) {
		PixelDentch* tempRef = &fullDentchGrid[start_x + (start_y * WIDTH)];
		addToAvailArrayDentch(tempRef);

		return tempRef;
	}

	std::atomic<int> curAtomicMinValue{ std::numeric_limits<int>::max() };
	std::atomic<int> curAtomicMinPtr{ 0 };

	std::for_each(std::execution::par, dentchAvailArray.begin(), dentchAvailArray.end(),
		[&](PixelDentch* wham) {
			int count = 0;
			int rSum = 0, gSum = 0, bSum = 0;
			Colour avgColour;
			Colour nColours[8];

			for (int i = 0; i < 8; i++) {
				if (wham->neighbors[i] != 0) {
					int nx = wham->x + dx[i];
					int ny = wham->y + dy[i];

					if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
						int neighborIndex = nx + ny * WIDTH;
						Colour col = fullDentchGrid[neighborIndex].col;
						nColours[i] = col;

						rSum += col.r;
						gSum += col.g;
						bSum += col.b;

						count++;
					}
				}
			}
			if (count == 0) { count = 1; }

			if (count == 4) {
				avgColour.r = nColours[0].r + nColours[1].r / 2;
				avgColour.g = nColours[0].g + nColours[1].g / 2;
				avgColour.b = nColours[0].b + nColours[1].b / 2;
			}
			else {
				rSum /= count;
				gSum /= count;
				bSum /= count;

				avgColour = {
					static_cast<uint8_t>(rSum),
					static_cast<uint8_t>(gSum),
					static_cast<uint8_t>(bSum)
				};
			}
			
			int colourDiff = colourDifference(avgColour, toCompare);

			int curVal = curAtomicMinValue.load(std::memory_order_relaxed);
			while (colourDiff < curVal && !curAtomicMinValue.compare_exchange_weak(curVal, colourDiff, std::memory_order_relaxed)) {
				// retry if another thread already updated with a closer match
			}

			if (colourDiff == curAtomicMinValue.load(std::memory_order_relaxed)) {
				curAtomicMinPtr.store(wham->index, std::memory_order_relaxed);
			}
		});

	return dentchAvailArray[curAtomicMinPtr];
}

PixelDentch* RenderEngine::parallel_MinDiff(Colour toCompare, int i) {
	if (i == 0 && stackLayers == 0) {
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

void RenderEngine::renderFrame(int layer, int count) {
	cv::Mat newFrame = cv::Mat(HEIGHT, WIDTH, CV_8UC3);
	for (int i = 0; i < HEIGHT * WIDTH; i++) {
		if (!fullDentchGrid[i].isEmpty) {
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].y, fullDentchGrid[i].x)[0] = fullDentchGrid[i].col.b;
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].y, fullDentchGrid[i].x)[1] = fullDentchGrid[i].col.g;
			newFrame.at<cv::Vec3b>(fullDentchGrid[i].y, fullDentchGrid[i].x)[2] = fullDentchGrid[i].col.r;
		}
	}
	cv::imwrite("D:\\CodingProjects\\FrameSplashCpp\\FrameSplashCpp\\x64\\Debug\\render\\Output\\l-" + std::to_string(layer) + "-frame" + std::to_string(count) + ".jpg", newFrame);
}

void RenderEngine::addToAvailArray(Pixel* px) {
	initPixel(px);
	availArray.push_back(px);
	px->index = availArray.size() - 1;
	px->isAvailable = true;
}

void RenderEngine::addToAvailArrayDentch(PixelDentch* px) {
	auto it = std::find(dentchAvailArray.begin(), dentchAvailArray.end(), px);
	if (it == dentchAvailArray.end()) {
		dentchAvailArray.push_back(px);
		px->index = dentchAvailArray.size() - 1;
		px->isAvailable = true;
	}
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

void RenderEngine::printMemoryAddress(Pixel *px, std::string msg) {
	std::cout << msg << " Memory Address: " << px << std::endl;
}

std::vector<PixelDentch*> RenderEngine::GetCirclePoints(int centreX, int centreY, int radius) {
	std::vector<PixelDentch*> circlePoints;
	int r2 = radius * radius;
	const int epsilon = radius;

	for (int i = centreX - radius; i <= centreX + radius; i++) {
		for (int j = centreY - radius; j < centreY + radius; j++) {
			if (i >= 0 && i < WIDTH && j >= 0 && j < HEIGHT) {
				int dx = i - centreX;
				int dy = j - centreY;
				int dist2 = dx * dx + dy * dy;

				if (std::abs(dist2 - r2) <= epsilon) {
					int index = i + (j * WIDTH);
					circlePoints.emplace_back(&fullDentchGrid[index]);
				}
			}
		}
	}

	return circlePoints;
}

std::vector<PixelDentch*> RenderEngine::GetCanvasOutlinePoints() {
	std::vector<PixelDentch*> outlinePoints;
	for(int x = 0; x < WIDTH; x++){
		int indexLeft = x;
		int indexRight = x + ((HEIGHT - 1) * WIDTH);

		outlinePoints.emplace_back(&fullDentchGrid[indexLeft]);
		outlinePoints.emplace_back(&fullDentchGrid[indexRight]);
	}
	for (int y = 1; y < HEIGHT-1; y++) {
		int indexTop = (y * WIDTH);
		int indexBottom = (y * WIDTH) - 1;

		outlinePoints.emplace_back(&fullDentchGrid[indexTop]);
		outlinePoints.emplace_back(&fullDentchGrid[indexBottom]);
	}

	return outlinePoints;
}

void RenderEngine::runMainLoop(int resolutionOffset) {
	int checkpoint = 1.f * ((HEIGHT * WIDTH) / 100); // percentage * totalsize/100
	int frameCounter = 0;
	int startingIndex = 0;

	std::cout << "Running main loop..." << std::endl;
	if (TEMPLATE && stackLayers == 0) {
		int templateCount = 0;
		std::vector<PixelDentch*> circlePoints = GetCirclePoints(WIDTH / 2, HEIGHT / 2, (HEIGHT/2) - 25);
		std::vector<PixelDentch*> circle2Points = GetCirclePoints(WIDTH / 2, HEIGHT / 2, (HEIGHT/2) - 27);
		//std::vector<PixelDentch*> outlinePoints = GetCanvasOutlinePoints();

		int circleSize = circlePoints.size();
		int circle2Size = circle2Points.size();
		//int outlineSize = outlinePoints.size();
		int maxSize = circleSize + circle2Size;// +outlineSize;

		int circleCount = 0;
		int circle2Count = 0;

		int outlineCount = 0;

		std::vector<Colour> orderedColours;
		for (int i = 0; i < maxSize; i++) {
			orderedColours.push_back(colourStack[i]);
		}
		std::sort(orderedColours.begin(), orderedColours.end(), [](const Colour& a, const Colour& b) {
			return static_cast<int>(a.r) + static_cast<int>(a.g) + static_cast<int>(a.b) < static_cast<int>(b.r) + static_cast<int>(b.g) + static_cast<int>(b.b);
		});

		while (templateCount < maxSize) {
			Colour col = orderedColours[templateCount];
			int index = -1;
			PixelDentch* px = nullptr;
			if (circle2Count < circle2Size) {
				px = circle2Points[circle2Count++];
				index = px->x + (px->y * WIDTH);
			}
			else if (circleCount < circleSize) {
				px = circlePoints[circleCount++];
				index = px->x + (px->y * WIDTH);
			}
			if (index >= 0) {
				fullDentchGrid[index].col = col;
				fullDentchGrid[index].isAvailable = false;
				fullDentchGrid[index].isEmpty = false;
			}
			templateCount++;
		}

		for (int i = 0; i < WIDTH * HEIGHT; i++) {
			if (!fullDentchGrid[i].isEmpty) {
				PixelDentch* px = &fullDentchGrid[i];
				for (int i = 0; i < 8; i++) {
					int nx = px->x + dx[i];
					int ny = px->y + dy[i];

					if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT)
					{
						int neighborIndex = nx + ny * WIDTH;
						fullDentchGrid[neighborIndex].neighbors[inverseIndex[i]] = 1;
						if (fullDentchGrid[neighborIndex].isEmpty)
						{
							addToAvailArrayDentch(&fullDentchGrid[neighborIndex]);
							fullDentchGrid[neighborIndex].neighbors[i] = 0;
						}
						else {
							fullDentchGrid[neighborIndex].neighbors[i] = 1;
						}
					}
				}
			}
		}

		renderFrame(stackLayers, frameCounter++);
		startingIndex += templateCount;
	}

	for (int i = startingIndex; i < (HEIGHT * WIDTH) - resolutionOffset; i++) {
		PixelDentch* curPXD;

		if (PARALLEL) {
			if (AVERAGE) {
				curPXD = parallel_AvgMinDiff(colourStack[i], i);
			}
			else {
				curPXD = parallel_MinDiff(colourStack[i], i);
			}
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
			std::cout << std::to_string((double)i / WIDTH / HEIGHT) << " stack: " << dentchAvailArray.size() << std::endl;

		int index = curPXD->x + (curPXD->y * WIDTH);

		assert(index >= 0 && index < WIDTH * HEIGHT);
		assert(fullDentchGrid[index].isEmpty);

		fullDentchGrid[index].col = colourStack[i];
		fullDentchGrid[index].isEmpty = false;

		removeFromAvailArrayDentch(curPXD);
		fullDentchGrid[index].isAvailable = false;

		addNeighboursToAvailArray(curPXD);

		if (i % checkpoint == 0 && i > 0) {
			renderFrame(stackLayers, frameCounter++);
		}
	}
	renderFrame(stackLayers, frameCounter++);

	if (STACKING_RESLUTION && stackLayers < STACK_LAYERS) {
		//Initialize a double size pixel grid and move all of the exzisting pixels onto it.
		int resolutionOffset = WIDTH * HEIGHT;
		initStackedCanvas(4);
		//run main loop again.
		stackLayers++;
		runMainLoop(resolutionOffset);
	}
}
