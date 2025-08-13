#include "PerlinReader.h"

//void PerlinReader::init(int height, int width) {
//    std::string filename = std::to_string(width) + "x" + std::to_string(height) + ".png";
//	perlinImage = cv::imread(dataBankDir + filename);
//    std::cout << "break" << std::endl;
//}

void PerlinReader::generate(int height, int width) {
    perlinImage = cv::Mat(height, width, CV_32FC1);
    cv::Mat perlinCpy = cv::Mat(height, width, CV_8UC3);

    initializePermutation();  // Generate permutation table

    // Scale controls frequency of noise
    float scale = 0.05f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float nx = x * scale;
            float ny = y * scale;
            float noiseValue = perlin(nx, ny);
            perlinImage.at<float>(y, x) = (noiseValue + 1.0f) * 0.5f; // Normalize to [0,1]

            perlinCpy.at<cv::Vec3b>(y, x)[0] = 255.f * ((noiseValue + 1.0f) * 0.5f);
            perlinCpy.at<cv::Vec3b>(y, x)[1] = 255.f * ((noiseValue + 1.0f) * 0.5f);
            perlinCpy.at<cv::Vec3b>(y, x)[2] = 255.f * ((noiseValue + 1.0f) * 0.5f);
        }
    }

    cv::imwrite("D:\\CodingProjects\\FrameSplashCpp\\FrameSplashCpp\\x64\\Debug\\render\\perlin.jpg", perlinCpy);
}

void PerlinReader::initializePermutation() {
    p.resize(256);
    std::iota(p.begin(), p.end(), 0);
    std::default_random_engine engine(std::random_device{}());
    std::shuffle(p.begin(), p.end(), engine);
    p.insert(p.end(), p.begin(), p.end());  // Duplicate the permutation vector
}


float PerlinReader::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinReader::lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float PerlinReader::grad(int hash, float x, float y) {
    int h = hash & 3;  // Only need 4 gradient directions
    float u = (h & 1) ? -x : x;
    float v = (h & 2) ? -y : y;
    return u + v;

    //int h = hash & 7;       // Convert low 3 bits of hash code
    //float u = h < 4 ? x : y;
    //float v = h < 4 ? y : x;
    //return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

float PerlinReader::perlin(float x, float y) {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    float xf = x - std::floor(x);
    float yf = y - std::floor(y);

    float u = fade(xf);
    float v = fade(yf);

    int aa = p[p[X] + Y];
    int ab = p[p[X] + Y + 1];
    int ba = p[p[X + 1] + Y];
    int bb = p[p[X + 1] + Y + 1];

    float x1 = lerp(u, grad(aa, xf, yf), grad(ba, xf - 1, yf));
    float x2 = lerp(u, grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1));
    return lerp(v, x1, x2);
}

float PerlinReader::getNoiseValue(int x, int y) {
    float col = perlinImage.data[perlinImage.channels() * (perlinImage.cols * y + x) + 0];
	return col / 255.f;
}

std::pair<int, int>  PerlinReader::getOffsetCoords(int x, int y) {
    double noiseValue = getNoiseValue(x, y);
    if (0.0625 < noiseValue && noiseValue < 0.1875) { return std::pair<int, int>(1, 1); }
    if (0.1875 < noiseValue && noiseValue < 0.3125) { return std::pair<int, int>(1, 0); }
    if (0.3125 < noiseValue && noiseValue < 0.4375) { return std::pair<int, int>(1, -1); }
    if (0.4375 < noiseValue && noiseValue < 0.5625) { return std::pair<int, int>(0, -1); }
    if (0.5625 < noiseValue && noiseValue < 0.6875) { return std::pair<int, int>(-1, -1); }
    if (0.6875 < noiseValue && noiseValue < 0.8125) { return std::pair<int, int>(-1, 0); }
    if (0.8125 < noiseValue && noiseValue < 0.9375) { return std::pair<int, int>(-1, 1); }
    if (0.0625 > noiseValue || noiseValue > 0.9375) { return std::pair<int, int>(0, 1); }
    return std::pair<int, int>(0, 0);
}