#include "DataManager.h"

DataManager::DataManager() {
}

void DataManager::randomiseParsedColours(int size) {
    std::shuffle(parsedColourData, parsedColourData+size, std::default_random_engine(0));
}

void DataManager::initColourCube() {
    std::cout << "Generating Colour Cube.." << std::endl;
    for (int i = 0; i < 256; ++i) {
        colourCube[i] = new unsigned int* [256];
        for (int j = 0; j < 256; ++j) {
            colourCube[i][j] = new unsigned int[256];
        }
    }

    for (int r = 0; r < 256; r++) {
        for (int g = 0; g < 256; g++) {
            for (int b = 0; b < 256; b++) {
                colourCube[r][g][b] = 0;
            }
        }
    }
}

void DataManager::videoToFrames() {
    cv::Mat frame;
    cv::VideoCapture cap(dataBankDir + "\\Assets\\video.mov");
    if (!cap.isOpened())
    {
        std::cout << "Error can't find the file" << std::endl;
    }
    else {
        int i = 0;
        int counter = 0;
        int numFramesTotal = cap.get(CAP_PROP_FRAME_COUNT);
        while (i < numFramesTotal) {
            cap >> frame;
            if(i % 10 == 0){ 
                std::cout << "Writing frame " << counter << "..." << std::endl;
                imwrite(dataBankDir + "\\Assets\\frames\\frame" + std::to_string(counter) + ".jpg", frame);
                counter++;
            }
            i++;
        }
    }
}

void DataManager::parseFrame(Mat frame) {
    for (int y = 0; y < frame.rows; y++) {
        for (int x = 0; x < frame.cols; x++) {
            int b = frame.data[frame.channels() * (frame.cols * y + x) + 0];
            int g = frame.data[frame.channels() * (frame.cols * y + x) + 1];
            int r = frame.data[frame.channels() * (frame.cols * y + x) + 2];
            colourCube[r][g][b]++;
        }
    }
}

void DataManager::framesToData() {
    //for every frame in the directory, iterate through the pixels of the frame and add them to the 3D array.
    //Probably a GPU thing.
    int frameCounter = 0;
    initColourCube();
    Mat img = imread(dataBankDir + "\\Assets\\frames\\frame" + std::to_string(frameCounter) + ".png", IMREAD_COLOR);
    while (!img.empty()) {
        std::cout << "Parsing frame " << frameCounter << "..." << "\r";
        parseFrame(img);
        frameCounter++;
        img = imread(dataBankDir + "\\Assets\\frames\\frame" + std::to_string(frameCounter) + ".png", IMREAD_COLOR);
    }
    if (frameCounter == 0) {
        std::cout << "Error: frames not found." << std::endl;
    }
}

void DataManager::exportData() {
    //save the colour cube to a txt file.
    std::ofstream out(dataBankDir + "\\Data\\data.txt");
    std::string dataPoint;
    int totalPixels = 0;
    int cumulativePixels = 0;
    std::cout << "Loading data to file..." << std::endl;
    for (int r = 0; r < 256; r++) {
        for (int g = 0; g < 256; g++) {
            for (int b = 0; b < 256; b++) {
                if (colourCube[r][g][b] > 0) {
                    dataPoint = "RGB(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + ")-" + std::to_string(colourCube[r][g][b]) + "\r\n";
                    out << dataPoint;
                    totalPixels++;
                    cumulativePixels += colourCube[r][g][b];
                }
            }
        }
    }
    out << "T:" + std::to_string(totalPixels) + "-C:" + std::to_string(cumulativePixels);
    std::cout << "File saved succesfully." << std::endl;
    out.close();
}

Colour DataManager::parseColourInput(std::string c) {
    try
    {
        int indexStart = c.find('(') + 1;
        int indexEnd = c.find(')');
        int occurance = c.find('-');

        string colorData = c.substr(indexStart, indexEnd - indexStart);
        string colorOccurance = c.substr(occurance+1, c.size() - occurance - 1);
        stringstream ss(colorData);
        vector<string> result;

        while (ss.good())
        {
            string substr;
            getline(ss, substr, ',');
            result.push_back(substr);
        }
        //string colors = colorData.Split('\u002C');
        Colour col = Colour(std::stoi(result[0]), std::stoi(result[1]), std::stoi((result[2])));
        return col;
    }
    catch (Exception ex)
    {
        std::cout << "Couldn't read colour data " << std::endl;
        return Colour(0, 0, 0);
    }
}

std::pair<Colour, int> DataManager::parseColourInputWithOccurance(std::string c) {
    try
    {
        int indexStart = c.find('(') + 1;
        int indexEnd = c.find(')');
        int occuranceStart = c.find('-');

        string colorData = c.substr(indexStart, indexEnd - indexStart);
        string colorOccurance = c.substr(occuranceStart + 1, c.size() - occuranceStart - 1);

        int occurance = std::stoi(colorOccurance);

        stringstream ss(colorData);
        vector<string> result;

        while (ss.good())
        {
            string substr;
            getline(ss, substr, ',');
            result.push_back(substr);
        }

        Colour col = Colour(std::stoi(result[0]), std::stoi(result[1]), std::stoi((result[2])));

        return std::make_pair(col, occurance);;
    }
    catch (Exception ex)
    {
        std::cout << "Couldn't read colour data " << std::endl;
        return std::make_pair(Colour(0, 0, 0), 0);
    }
}

Colour* DataManager::importData(int height, int width) {
    try {
        std::ifstream file(dataBankDir + "\\Data\\data.txt");
        std::string colourInfo;
        parsedColourVector = vector<Colour>();
        while (std::getline(file, colourInfo)) {
            if (colourInfo.find('RGB') != std::string::npos) {
                //parsedColourVector.push_back(parseColourInput(colourInfo));
                auto result = parseColourInputWithOccurance(colourInfo);
                //if (result.second > (width*height/100.f) * 1.f) {
                //    continue;
                //}
                for (int i = 0; i < result.second; i++) {
                    parsedColourVector.push_back(result.first);
                }
            }
        }
        int ptr = 0;
        parsedColourData = new Colour[parsedColourVector.size()];
        NumIndividualColours = parsedColourVector.size();
        std::cout << "Num Colours: " << NumIndividualColours << std::endl;
        std::cout << "Recommended resolution: " << sqrt(NumIndividualColours) << std::endl;

        for (Colour elem : parsedColourVector) {
            parsedColourData[ptr] = elem;
            ptr++;
        }
        return parsedColourData;
    }
    catch (Exception e) {
        std::cout << "Error reading file." << std::endl;
        std::cout << e.msg << std::endl;
    }
}


Colour* DataManager::getDefaultRGBData(int height, int width) {
    int size = height * width;
    Colour* arr = new Colour[size];
    int occurance = 1;

    int counter = 0;
    for (int i = 0; i < height-1; i++) {
        for (int j = 0; j < width-1; j++) {
            int r = 255.f * ((float)i / (float)height);
            int g = 255.f * ((float)j / (float)width);
            arr[counter] = Colour(g, (int)(r + g / 2), (int)(r + g / 2));

            counter++;
        }
    }
    return arr;
}

//type can be rgb for a uniform distribution of 0..255 rgb values.
//or 'import' where the colours are inputted 
Colour* DataManager::getParsedData(int height, int width, std::string type) {
    if (type == "rgb") {
        parsedColourData = getDefaultRGBData(height, width);
    }
    else if (type == "file") {
        parsedColourData = importData(height, width);
    }
    std::cout << "colour size: " << height * width << std::endl;
    randomiseParsedColours(NumIndividualColours);
    return parsedColourData;
}
