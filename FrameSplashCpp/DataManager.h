#ifndef DATA_MANAGER
#define DATA_MANAGER

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <algorithm>
#include <execution>
#include <vector>
#include <numeric>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Helper.h"
#include "TemplateEngine.h"
#include "Enums.h"

using namespace cv;

class DataManager {
private:
	std::string dataBankDir = "D:\\CodingProjects\\FrameSplashCpp\\FrameSplashCpp\\x64\\Debug\\render"; //Fix this to not be hardcoded.
	unsigned int*** colourCube = new unsigned int** [256];
	Colour* parsedColourData;
	std::vector<Colour> parsedColourVector;

	void InitColourCube();
	void ParseFrame(cv::Mat frame);
	
	Colour* GetDefaultRGBData(int height, int width, ColourPreprocessing preprocessingType);
	Colour* GetCustomRGBData(int height, int width, ColourPreprocessing preprocessingType);
	void RandomiseParsedColours(size_t size, size_t blockSize = 10000);
public:
	DataManager();
	void VideoToFrames();
	void FramesToData();
	void ExportData();
	Colour* ImportDataAsRaw(int height, int width);
	Colour* ImportShuffledData(int height, int width);
	Colour* ImportWeightedData(int height, int width);
	
	
	Colour* ImportDataFromFile(int height, int width, ColourPreprocessing preprocessing);

	Colour* GetParsedData(int height, int width, ColourDataType type, ColourPreprocessing preprocessing);
	Colour ParseColourStringFromFile(std::string c);
	std::pair<Colour, int> ParseColourInputWithOccurance(std::string c);
};

#endif


