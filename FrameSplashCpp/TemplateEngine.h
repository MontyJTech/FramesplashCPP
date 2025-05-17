#ifndef TEMPLATE_ENGINE
#define TEMPLATE_ENGINE

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Helper.h"

struct Stupid {
    cv::Mat img;
    std::vector<std::pair<Colour, Pixel>> temp;
    Stupid(cv::Mat img, std::vector<std::pair<Colour, Pixel>> temp) {
        this->img = img;
        this->temp = temp;
    }
};

using namespace cv;
//using std::cout; using std::endl;
using namespace std;
class TemplateEngine {
private:
	std::vector<std::pair<Colour, Pixel>> templateData;
public:
    TemplateEngine();
	void openWindow();
    void addPixelToTemplateData(std::pair<Colour, Pixel> data);
    
    static void mouseCallback(int event, int x, int y, int flags, void* userdata) {
        if (event == EVENT_LBUTTONDOWN)
        {
            Stupid* what = (Stupid*)userdata;
            cv::Mat* snapshot = (cv::Mat*)&what->img;
            cv::Vec3b pixel = snapshot->at<cv::Vec3b>(cv::Point(x, y));
            Pixel tempPx = Pixel(x, y);
            Colour tempColour = Colour(pixel[2], pixel[1], pixel[0]);
            what->temp.push_back(std::pair<Colour, Pixel>(tempColour, tempPx));
            //templateData.push_back(std::pair<Colour, Pixel>(tempColour, tempPx));
            int b, g, r;
            b = pixel[0];
            g = pixel[1];
            r = pixel[2];
            cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
            cout << "Left button of the mouse is clicked - colour (" << std::to_string(r) << ", " << std::to_string(g) << ", " << std::to_string(b) <<")" << endl;
        }
    }


	//if using temp engine, the engine initialises by loading in a base image for the user to click.
	//For every click from the user, the coordinates and colour of the pixel are recorded in a vector.
	//Once the user has finished clicking, they press a key to close the window
	//At which point, this data is sent to render engine to place.
};

#endif 