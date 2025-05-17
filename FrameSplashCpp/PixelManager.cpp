#include "PixelManager.h"

void PixelManager::initFilledNeighbours(Pixel *px, Pixel* pixelList, int size) {
	for (int i = 0; i < size; i++) {
		px->filledPixelList.push_back(&pixelList[i]);
	}
}

void PixelManager::initEmptyNeighbours(Pixel *px, Pixel* pixelList, int size) {
	for (int i = 0; i < size; i++) {
		px->emptyPixelList.push_back(&pixelList[i]);
	}
}

void PixelManager::moveNeighbourToFilled(Pixel *px, Pixel* filledPx) {
	auto it = std::find(px->emptyPixelList.begin(), px->emptyPixelList.end(), filledPx);
	int index = it - px->emptyPixelList.begin();
	Pixel* temp = px->emptyPixelList.at(index);
	px->emptyPixelList.erase(px->emptyPixelList.begin() + index);
	px->filledPixelList.push_back(temp);
}

int PixelManager::isInVector(std::vector<Pixel*> *list, Pixel* px) {
	auto it = std::find(list->begin(), list->end(), px);
	int index = it - list->begin();
	if (it == list->end()) {
		return -1;
	}
	else {
		return index;
	}
}

void PixelManager::updateAllNeighbours(Pixel* px) {
	//std::cout << "5 Mem address of Current PX: " << px << std::endl;
	for (Pixel* elem : px->emptyPixelList) {
		int index = isInVector(&(elem->emptyPixelList), px);
		if (index > 0) {
			elem->emptyPixelList.erase(elem->emptyPixelList.begin()+index);
			elem->filledPixelList.push_back(px);
		}
	}
	for (Pixel* elem : px->filledPixelList) {
		int index = isInVector(&(elem->emptyPixelList), px);
		if (index > 0) {
			elem->emptyPixelList.erase(elem->emptyPixelList.begin()+index);
			elem->filledPixelList.push_back(px);
		}
	}
}