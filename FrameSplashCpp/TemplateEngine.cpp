#include "TemplateEngine.h"

void TemplateEngine::openWindow() {
    std::string image_path = "D:\\nonesense\\FrameSplashCPP\\FrameSplashCpp\\render\\Assets\\TemplateFrame\\TemplateFrame.jpg";
    Mat img = imread(image_path, IMREAD_COLOR);
    if (img.empty())
    {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return;
    }
    Stupid dumb = Stupid(img, templateData);
    namedWindow("My Window", 1);
    setMouseCallback("My Window", TemplateEngine::mouseCallback, &dumb);
    for (int i = 0; i < templateData.size(); i++) {
        std::cout << templateData[i].first.r << " - " << templateData[i].second.x << std::endl;
    }
    imshow("My Window", img);

    int k = waitKey(0); // Wait for a keystroke in the window
}

void TemplateEngine::addPixelToTemplateData(std::pair<Colour, Pixel> data) {
    templateData.push_back(data);
}