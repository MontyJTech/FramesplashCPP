//#include "Pixel.h"
//
//Pixel::Pixel(int x, int y) {
//	this->x = x;
//	this->y = y;
//	col = Colour(0, 0, 0);
//}
//
//Pixel::Pixel() {
//	this->x = 0;
//	this->y = 0;
//	col = Colour(0, 0, 0);
//}
//
//void Pixel::assertNUninitialized() {
//	assert(numFilledNeighbours == 0);
//	assert(numFilledNeighbours == 0);
//}
//
//void Pixel::initFilledNeighbours(Pixel** pixelList, int size) {
//	assertNUninitialized();
//	neighboursFilled = new Pixel*[8];
//
//	for (int i = 0; i < size; i++) {
//		neighboursFilled[i] = pixelList[i];
//	}
//	numFilledNeighbours = size;
//}
//
//void Pixel::initEmptyNeighbours(Pixel** pixelList, int size) {
//	assertNUninitialized();
//	neighboursEmpty = new Pixel*[8];
//
//	for (int i = 0; i < size; i++) {
//		neighboursEmpty[i] = pixelList[i];
//	}
//	numEmptyNeighbours = size;
//}
//
//bool Pixel::getIsEmpty() {
//	return isEmpty;
//}
//
//void Pixel::setIsEmpty(bool newIsEmpty) {
//	isEmpty = newIsEmpty;
//}
//
//int Pixel::getIndex() {
//	return index;
//}
//
//void Pixel::setIndex(int index) {
//	this->index = index;
//}
//
//void Pixel::destruct() {
//	delete[] neighboursFilled;
//	delete[] neighboursEmpty;
//}
//
//void Pixel::initNAvg() {
//
//}
//
//void Pixel::editNAvg(Colour c) {
//
//}
//
