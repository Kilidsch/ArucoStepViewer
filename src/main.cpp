#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <FL/Fl.H>

#include "aruco.h"
#include "gui.h"

#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Input.H>

void simulateCallback(Fl_Widget *, void *data)
{
    auto cbData = (SimulateCallbackData *)data;
    auto userParams = cbData->pmWin->getArucoParameters();
    auto &img = cbData->img;

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    auto previewImgs = simulateDetectMarkers(img, dictionary, markerCorners, markerIds, userParams, rejectedCandidates);

    cv::Mat rgbImg;
    cv::cvtColor(img, rgbImg, cv::COLOR_BGR2RGB);
    cbData->win.init({{"test", {img}},
                      {"testRGB", {rgbImg}},
                      {"Thresholds", previewImgs.threshold},
                      {"Contours", previewImgs.contours},
                      {"Raw Contours", previewImgs.contoursRaw},
                      {"Perimeter", previewImgs.contoursPerimeter},
                      {"Square", previewImgs.contoursSquareConvex},
                      {"MinDist", previewImgs.contoursMinDist}});
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    std::string path = R"(/home/deniz/Bilder/cake1080p2.png)";
    auto img = cv::imread(path);
    //cv::medianBlur(img, img, 5);

    // cv::resize(img, img, {0, 0}, 0.2, 0.2);
    cv::Mat rgbImg;
    cv::cvtColor(img, rgbImg, cv::COLOR_BGR2RGB);

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    auto previewImgs = simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

    [[maybe_unused]] auto imageWindow = ImageWindow(argc, argv,
                                                    {{"test", {img}},
                                                     {"testRGB", {rgbImg}},
                                                     {"Thresholds", previewImgs.threshold},
                                                     {"Contours", previewImgs.contours},
                                                     {"Raw Contours", previewImgs.contoursRaw},
                                                     {"Perimeter", previewImgs.contoursPerimeter},
                                                     {"Square", previewImgs.contoursSquareConvex},
                                                     {"MinDist", previewImgs.contoursMinDist}});

    SimulateCallbackData data{img, imageWindow};
    [[maybe_unused]] auto parameterWindow = ParameterWindow(argc, argv, simulateCallback, data);
    return Fl::run();
}
