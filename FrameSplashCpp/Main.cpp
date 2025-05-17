#include <iostream>
#include "DataManager.h"
#include "RenderEngine.h"

//4k: 3840x2160
const int WIDTH = 1000;
const int HEIGHT = 1000;
int main() {

	//1. if working with video, convert to frames.
	DataManager dataMgr = DataManager();
	RenderEngine renderEng = RenderEngine();

	//dataMgr.videoToFrames();
	//dataMgr.framesToData();
	//dataMgr.exportData();

	Colour* stack = dataMgr.getParsedData(HEIGHT, WIDTH, "file");
	renderEng.init(HEIGHT, WIDTH, stack);
	renderEng.runMainLoop();
	
	return 0;
}