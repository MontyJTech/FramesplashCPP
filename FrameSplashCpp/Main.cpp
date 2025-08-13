#include <iostream>
#include "DataManager.h"
#include "RenderEngine.h"

const int WIDTH = 1920;
const int HEIGHT = 1080;

int main() {
	DataManager dataMgr = DataManager();
	RenderEngine renderEng = RenderEngine();

	//dataMgr.videoToFrames(); //If working with video, convert to frames first (needed?)
	//dataMgr.framesToData();
	//dataMgr.exportData();

	//Colour* stack = dataMgr.getParsedData(HEIGHT, WIDTH, "file");
	std::cout << "Loading Data..." << std::endl;
	Colour* stack = dataMgr.getParsedData(HEIGHT, WIDTH, ColourDataType::LOADED, ColourPreprocessing::RANDOM_SHUFFLE);
	renderEng.init(HEIGHT, WIDTH, stack);
	renderEng.runMainLoop();
	
	return 0;
}