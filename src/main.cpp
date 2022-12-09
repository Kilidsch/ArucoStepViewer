#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>

#include <FL/Fl.H>

#include "gui.h"
#include "aruco.h"

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

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    // parameters->adaptiveThreshWinSizeMin = 10;
    // parameters->adaptiveThreshWinSizeMax = 400;
    // parameters->adaptiveThreshWinSizeStep = 20;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    auto threshImgs = simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

    [[maybe_unused]] auto parameterWindow = ParameterWindow(argc, argv, myCallback);
    [[maybe_unused]] auto imageWindow = ImageWindow(argc, argv, {{"test", {img}}, {"testRGB", {rgbImg}}, {"Thresholds", threshImgs}});

    return Fl::run();
}
