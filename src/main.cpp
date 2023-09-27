#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/imgcodecs.hpp>

#include "aruco.h" // for simulateDetectMarkers
#include "arucoparamscontroller.h"
#include "imagemodel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Fusion");

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

    ImageModel model;
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
}
