#ifndef PERLIN_READER
#define PERLIN_READER

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>

class PerlinReader {
public:
	cv::Mat perlinImage;

	//void init(int height, int width);
	void generate(int height, int width);
	float getNoiseValue(int x, int y);
	std::pair<int, int> getOffsetCoords(int x, int y);

private:
	//std::string dataBankDir = "C:\\Users\\Monty\\Desktop\\NFTTest\\sln\\FrameSplashCpp\\FrameSplashCpp\\x64\\Debug\\perlinNoise\\";

	std::vector<int> p;  // Permutation vector

	// Helpers for Perlin noise
	float fade(float t);
	float lerp(float t, float a, float b);
	float grad(int hash, float x, float y);
	float perlin(float x, float y);

	void initializePermutation();
};

#endif