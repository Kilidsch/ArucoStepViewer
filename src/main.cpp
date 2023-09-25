#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <FL/Fl.H>

#include "aruco.h"
#include "imagemodel.h"

#include "arucoparamscontroller.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

// void simulateCallback(Fl_Widget *, void *data)
//{
//     auto cbData = (SimulateCallbackData *)data;
//     auto userParams = cbData->pmWin->getArucoParameters();
//     auto &img = cbData->img;

//    std::vector<int> markerIds;
//    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
//    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
//    simulateDetectMarkers(img, dictionary, markerCorners, markerIds, userParams, rejectedCandidates);

//    cv::Mat rgbImg;
//    cv::cvtColor(img, rgbImg, cv::COLOR_BGR2RGB);
//    cbData->win.init(TestImages::getInstance().getTabs());
//}
#include <QTimer>
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Fusion");
    ImageModel model;

    std::string path;
    if (argc > 1)
    {
        path = argv[1];
    }
    else
    {
        path = R"(D:\KleineProjekte\ArucoStepViewer\test.png)";
    }
    auto img = cv::imread(path);
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

    model.setTabs(TestImages::getInstance().getTabs());

    ArucoParamsController arucoController;
    QObject::connect(&arucoController, &ArucoParamsController::paramsChanged, [&]() {
        // re-run with new parameters; images are reset in simulateDetectMarkers
        auto params = arucoController.getParams();
        auto parameters = std::make_shared<cv::aruco::DetectorParameters>(params);
        simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
        // model updates the UI
        model.setTabs(TestImages::getInstance().getTabs());
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("ImageModel", &model);
    engine.rootContext()->setContextProperty("_aruco", &arucoController);
    engine.load(QUrl("qrc:/stepviewer/src/main.qml"));
    return app.exec();

    // -------------------------------------------------------------

    //cv::medianBlur(img, img, 5);

    // cv::resize(img, img, {0, 0}, 0.2, 0.2);
    cv::Mat rgbImg;
    cv::cvtColor(img, rgbImg, cv::COLOR_BGR2RGB);

    //[[maybe_unused]] auto imageWindow = ImageWindow(argc, argv, TestImages::getInstance().getTabs());

    // SimulateCallbackData data{img, imageWindow};
    //[[maybe_unused]] auto parameterWindow = ParameterWindow(argc, argv, simulateCallback, data);
    // return Fl::run();
}
