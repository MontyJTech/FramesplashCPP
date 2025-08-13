#include "DataManager.h"

DataManager::DataManager() {
}

void DataManager::RandomiseParsedColours(size_t size, size_t blockSize) {
    std::shuffle(parsedColourData, parsedColourData+size, std::default_random_engine(0));
}

void DataManager::InitColourCube() {
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

void DataManager::VideoToFrames() {
    cv::Mat frame;
    cv::VideoCapture cap(dataBankDir + "\\Assets\\video.mp4");
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

void DataManager::ParseFrame(Mat frame) {
    for (int y = 0; y < frame.rows; y++) {
        for (int x = 0; x < frame.cols; x++) {
            int b = frame.data[frame.channels() * (frame.cols * y + x) + 0];
            int g = frame.data[frame.channels() * (frame.cols * y + x) + 1];
            int r = frame.data[frame.channels() * (frame.cols * y + x) + 2];
            colourCube[r][g][b]++;
        }
    }
}

void DataManager::FramesToData() {
    //for every frame in the directory, iterate through the pixels of the frame and add them to the 3D array.
    //Probably a GPU thing.
    int numFrames = 48248;
    int frameCounter = 0;
    InitColourCube();
    for (int i = 0; i < numFrames; i++) {
        try {
            Mat img = imread(dataBankDir + "\\Assets\\frames\\frame" + std::to_string(i) + ".png", IMREAD_COLOR);
            ParseFrame(img);
            if (i % 100 == 0) {
                std::cout << "Parsing frame " << i << "/" << numFrames << "..." << "\r";
            }
        }
        catch (Exception e) {
        }
    }

    //while (!img.empty()) {
    //    std::cout << "Parsing frame " << frameCounter << "..." << "\r";
    //    parseFrame(img);
    //    frameCounter++;
    //    img = imread(dataBankDir + "\\Assets\\frames\\frame" + std::to_string(frameCounter) + ".png", IMREAD_COLOR);
    //}
    if (frameCounter == 0) {
        std::cout << "Error: frames not found." << std::endl;
    }
}

void DataManager::ExportData() {
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

Colour DataManager::ParseColourStringFromFile(std::string c) {
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

        Colour col = Colour(std::stoi(result[0]), std::stoi(result[1]), std::stoi((result[2])));
        return col;
    }
    catch (Exception ex)
    {
        std::cout << "Couldn't read colour data " << std::endl;
        return Colour(0, 0, 0);
    }
}

std::pair<Colour, int> DataManager::ParseColourInputWithOccurance(std::string c) {
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

Colour* DataManager::ImportDataFromFile(int height, int width, ColourPreprocessing preprocessing) {
    switch (preprocessing) {
        case ColourPreprocessing::NONE:
            return ImportDataAsRaw(height, width);
        case ColourPreprocessing::RANDOM_SHUFFLE:
            return ImportShuffledData(height, width);
        case ColourPreprocessing::WEIGHTED:
            return ImportWeightedData(height, width);
        default:
            return ImportShuffledData(height, width);
    }
}

Colour* DataManager::ImportDataAsRaw(int height, int width) {
    try {
        std::ifstream file(dataBankDir + "\\Data\\data.txt");
        if (!file) {
            std::cerr << "Failed to open file.\n";
            return nullptr;
        }

        std::string colourInfo;
        parsedColourData = new Colour[width * height];
        int coloursCount = 0;

        while (std::getline(file, colourInfo)) {
            if (colourInfo.find("RGB") != std::string::npos) {
                std::pair<Colour, int> result = ParseColourInputWithOccurance(colourInfo);
                for (int i = 0; i < result.second; i++) {
                    if (coloursCount == width * height) {
                        return parsedColourData;
                    }
                    parsedColourData[coloursCount] = result.first;
                    coloursCount++;
                }
            }
        }

        return parsedColourData;
    }
    catch (Exception e) {
        std::cout << "Error reading file." << std::endl;
        std::cout << e.msg << std::endl;
    }
}

Colour* DataManager::ImportShuffledData(int height, int width) {
    try {
        std::ifstream file(dataBankDir + "\\Data\\data.txt"); //Fix this to not be hardcoded.
        if (!file) {
            std::cerr << "Failed to open file.\n";
            return nullptr;
        }

        std::string colourInfo;
        std::vector<std::pair<Colour, int>> colourCounts;
        size_t totalCount = 0;
        size_t canvasCount = width * height;

        while (std::getline(file, colourInfo)) {
            if (colourInfo.find("RGB") != std::string::npos) {
                auto result = ParseColourInputWithOccurance(colourInfo);
                if (result.second > 0) {
                    colourCounts.emplace_back(result.first, result.second);
                    totalCount += result.second;
                }
            }
        }

        parsedColourData = new Colour[height * width];

        int pointer = 0;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::cout << "\n";

        while (pointer < canvasCount) {
            std::uniform_int_distribution<uint64_t> distrib(1, totalCount);
            uint64_t random_number = distrib(gen) - 1;

            uint64_t counter = 0;
            size_t ptr = 0;
            while (random_number > counter && ptr < colourCounts.size()) {
                counter += colourCounts[ptr].second;
                ptr++;
            }
            if (ptr == colourCounts.size()) {
                continue;
            }
            
            if (pointer % 1000 == 0) {
                std::cout << "\rRandom pick: " << std::to_string(((float)pointer / (float)canvasCount) * 100.f) << "%";
            }

            parsedColourData[pointer] = colourCounts[ptr].first;
            pointer++;
        }

        return parsedColourData;
    }
    catch (Exception e) {
        std::cout << "Error reading file." << std::endl;
        std::cout << e.msg << std::endl;
    }
}

Colour* DataManager::ImportWeightedData(int height, int width) {
    try {
        std::ifstream file(dataBankDir + "\\Data\\data.txt"); //Fix this to not be hardcoded.
        if (!file) {
            std::cerr << "Failed to open file.\n";
            return nullptr;
        }

        std::string colourInfo;
        std::vector<std::pair<Colour, int>> colourCounts;
        size_t fileColoursCount = 0;
        size_t canvasSize = width * height;

        while (std::getline(file, colourInfo)) {
            if (colourInfo.find("RGB") != std::string::npos) {
                auto result = ParseColourInputWithOccurance(colourInfo);
                if (result.second > 0) {
                    colourCounts.push_back(result);
                    fileColoursCount += result.second;
                }
            }
        }

        std::cout << "Read file successfully. \n";

        parsedColourData = new Colour[canvasSize];

        auto max_it = std::max_element(colourCounts.begin(), colourCounts.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            }
        );
        int maxColourIndex = std::distance(colourCounts.begin(), max_it);

        
        int addedColoursCount = 0;
        for (int i = colourCounts.size()-1; i >= 0; i--) {
            if (i == maxColourIndex) {
                continue;
            }
            float colourRatio = (float)colourCounts[i].second / (float)fileColoursCount;
            int numOccurances = colourRatio * (float)canvasSize;
            //if (numOccurances == 0)
            //    numOccurances++;
            while (numOccurances-- > 0) {
                parsedColourData[addedColoursCount] = colourCounts[i].first;
                addedColoursCount++;
                if (addedColoursCount % 1000 == 0) std::cout << "\rAdding colours: " << std::to_string(((float)addedColoursCount / (float)canvasSize) * 100.f) << "%";
            }
        }
        assert(addedColoursCount < canvasSize);

        while (addedColoursCount < canvasSize) {
            parsedColourData[addedColoursCount] = colourCounts[maxColourIndex].first;
            addedColoursCount++;
            if (addedColoursCount % 1000 == 0) std::cout << "\rAdding colours: " << std::to_string(((float)addedColoursCount / (float)canvasSize) * 100.f) << "%";
        }

        std::cout << "\nDone loading colours. \n";
        return parsedColourData;
    }
    catch (Exception e) {
        std::cout << "Error reading file." << std::endl;
        std::cout << e.msg << std::endl;
    }
}

//4096 x 4096 at full colour spectrum - 16,777,216 colours
Colour* DataManager::GetDefaultRGBData(int height, int width, ColourPreprocessing preprocessingType) {
    size_t size = height * width;

    int colourCubeIndex = std::cbrt(size);
    if (colourCubeIndex > 255)
        colourCubeIndex = 255;

    float colourIndexRatio = 255.f / (float)colourCubeIndex;

    size_t* indexOrder = new size_t[size];
    for (size_t i = 0; i < size; i++)
    {
        indexOrder[i] = i;
    }

    if (preprocessingType == ColourPreprocessing::RANDOM_SHUFFLE) {
        std::shuffle(indexOrder, indexOrder + size, std::default_random_engine(0));
    }

    Colour* arr = new Colour[size];
    int counter = 0;
    for (int rIndex = 0; rIndex < colourCubeIndex; rIndex++) {
        for (int gIndex = 0; gIndex < colourCubeIndex; gIndex++) {
            for (int bIndex = 0; bIndex < colourCubeIndex; bIndex++) {
                int r = rIndex * colourIndexRatio;
                int g = gIndex * colourIndexRatio;
                int b = bIndex * colourIndexRatio;

                arr[indexOrder[counter]] = Colour(r,g,b);
                counter++;
            }
        }
    }

    return arr;
}

Colour* DataManager::GetCustomRGBData(int height, int width, ColourPreprocessing preprocessingType) {
    size_t size = height * width;
    Colour* arr = new Colour[size];

    int counter = 0;
    for (int i = 0; i < height - 1; i++) {
        for (int j = 0; j < width - 1; j++) {
            int r = 255.f * ((float)i / (float)height);
            int g = 255.f * ((float)j / (float)width);
            arr[counter] = Colour(g, (int)(r + g / 2), (int)(r + g / 2));

            counter++;
        }
    }

    if (preprocessingType == ColourPreprocessing::RANDOM_SHUFFLE) {
        std::shuffle(arr, arr + size, std::default_random_engine(0));
    }

    return arr;
}

Colour* DataManager::GetParsedData(int height, int width, ColourDataType type, ColourPreprocessing preprocessing) {
    switch (type) {
        case ColourDataType::RGB:
            parsedColourData = GetDefaultRGBData(height, width, preprocessing);
            break;
        case ColourDataType::RGB_CUSTOM:
            parsedColourData = GetCustomRGBData(height, width, preprocessing);
            break;
        case ColourDataType::LOADED:
            parsedColourData = ImportDataFromFile(height, width, preprocessing);
            break;
        default:
            parsedColourData = GetDefaultRGBData(height, width, ColourPreprocessing::NONE);
            break;
    }

    std::cout << "actual resolution: " << height << ", " << width << std::endl;
    //randomiseParsedColours(NumIndividualColours);
    return parsedColourData;
}
