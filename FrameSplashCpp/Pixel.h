//#ifndef PIXEL
//#define PIXEL
//#include "Helper.h"
//#include <cassert>
//
//class Pixel {
//private:
//	bool isEmpty = true;
//	int index;
//	int numFilledNeighbours = 0;
//	int numEmptyNeighbours = 0;
//	Pixel** neighboursEmpty;
//	Pixel** neighboursFilled;
//
//public:
//	Colour col;
//	int x, y;
//	int nAvg = 0;
//
//	Pixel(int x, int y);
//	Pixel();
//
//	void initFilledNeighbours(Pixel** pixelList, int size);
//	void initEmptyNeighbours(Pixel** pixelList, int size);
//	//void moveNeighbourToFilled(Pixel& emptyPx, Pixel& filledPx);
//
//	void initNAvg();
//	void editNAvg(Colour c);
//
//	bool getIsEmpty();
//	int getIndex();
//
//	void setIsEmpty(bool newIsEmpty);
//	void setIndex(int index);
//
//	void assertNUninitialized();
//	void destruct();
//
//};
//
//#endif // !PIXEL