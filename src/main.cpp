#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>

#include <FL/Fl.H>

#include "gui.h"

#include <FL/Fl_Value_Input.H>
void myCallback(Fl_Widget *val, void *)
{
    std::cout << ((Fl_Value_Input *)val)->value() << std::endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    std::string path = R"(/home/deniz/images/test.jpg)";
    auto img = cv::imread(path);
    cv::resize(img, img, {0, 0}, 0.2, 0.2);
    cv::Mat rgbImg;
    cv::cvtColor(img, rgbImg, cv::COLOR_BGR2RGB);

    ParameterWindow win(argc, argv, myCallback);
    ImageWindow(argc, argv, img, rgbImg);
    
    return Fl::run();
}
