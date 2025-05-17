#ifndef DATA_MANAGER
#define DATA_MANAGER

#include <string>
#include <iostream>
#include <fstream>
#include <random>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Helper.h"
#include "TemplateEngine.h"

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
	Colour* getDefaultRGBData(int height, int width);
	void randomiseParsedColours(int size);
public:
	DataManager();
	void videoToFrames();
	void framesToData();
	void exportData();
	Colour* importData(int height, int width);
	Colour* getParsedData(int height, int width, std::string type = "rgb");

	int NumIndividualColours = 0;
};

#endif


