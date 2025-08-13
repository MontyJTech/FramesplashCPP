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
	std::string dataBankDir = "D:\\CodingProjects\\FrameSplashCpp\\FrameSplashCpp\\x64\\Debug\\render";
	unsigned int*** colourCube = new unsigned int** [256];
	Colour* parsedColourData;
	std::vector<Colour> parsedColourVector;

	void initColourCube();
	void parseFrame(cv::Mat frame);
	Colour parseColourInput(std::string c);
	std::pair<Colour, int> parseColourInputWithOccurance(std::string c);
	Colour* getDefaultRGBData(int height, int width, ColourPreprocessing preprocessingType);
	Colour* getCustomRGBData(int height, int width, ColourPreprocessing preprocessingType);
	void randomiseParsedColours(size_t size, size_t blockSize = 10000);
public:
	DataManager();
	void videoToFrames();
	void framesToData();
	void exportData();
	Colour* importDataAsRaw(int height, int width);
	Colour* importDataByWeight(int height, int width);
	Colour* getParsedData(int height, int width, ColourDataType type, ColourPreprocessing preprocessing);

	size_t NumIndividualColours = 0;
};

#endif


