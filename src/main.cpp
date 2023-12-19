#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "aruco.h" // for simulateDetectMarkers
#include "arucoparamscontroller.h"
#include "imagemodel.h"
#include "source.h"

#include <QCommandLineParser>
#include <QDeadlineTimer>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include <atomic>
#include <chrono>
#include <opencv2/videoio.hpp>
#include <string>
#include <thread>

using namespace std::chrono_literals;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Fusion");
    QCoreApplication::setApplicationName("ArucoStepViewer");

    // TODO: put into extra function, see docu for how to replace the `app` on parser.process
    QCommandLineParser parser;
    parser.setApplicationDescription("Read the README, I guess? (TODO)");
    parser.addHelpOption();

    QCommandLineOption image_option("image", "Input image", "image-path");
    parser.addOption(image_option);
    QCommandLineOption video_option("video", "Input video", "video-path");
    parser.addOption(video_option);

    parser.process(app);
    if (parser.isSet(image_option) && parser.isSet(video_option))
    {
        std::cerr << "Only one of the input options (image, video) can be used at once\n";
        exit(1);
    }
    std::string path;
    InputType type;
    if (!parser.isSet(image_option) && !parser.isSet(video_option))
    {
        std::cout << "No input given, using default value. Use --image or --video to run on other data" << std::endl;
        path = R"(D:\KleineProjekte\ArucoStepViewer\test.png)";
        type = InputType::Image;
    }
    else if (parser.isSet(image_option))
    {
        path = parser.value(image_option).toStdString();
        type = InputType::Image;
    }
    else
    {
        path = parser.value(video_option).toStdString();
        type = InputType::Video;
    }

    Source source(type, path);

    auto img = source.getImg();
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

    ImageModel model;
    model.setTabs(TestImages::getInstance().getTabs());

    ArucoParamsController arucoController;

    std::jthread thread;
    std::atomic<cv::aruco::DetectorParameters> curr_params;
    curr_params.store(arucoController.getParams());
    switch (type)
    {
    case InputType::Image:
        // When using an image, rerun on every change of parameters
        thread = std::jthread([&](std::stop_token stoken) {
            while (!stoken.stop_requested())
            {
                // re-run with new parameters; images are reset in simulateDetectMarkers
                auto params = curr_params.load();
                auto parameters = std::make_shared<cv::aruco::DetectorParameters>(params);
                simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
                // model updates the UI
                model.setTabs(TestImages::getInstance().getTabs());

                curr_params.wait(params);
            }
        });
        QObject::connect(&arucoController, &ArucoParamsController::paramsChanged, [&]() {
            curr_params.store(arucoController.getParams());
            curr_params.notify_all();
        });
        break;
    case InputType::Video:
        // When video(stream), rerun on every new frame
        thread = std::jthread([&](std::stop_token stoken) {
            while (!stoken.stop_requested())
            {
                QDeadlineTimer timer(20ms);
                // getImg is a blocking call and can limit the speed of this thread
                auto img = source.getImg();
                auto params = curr_params.load();
                auto parameters = std::make_shared<cv::aruco::DetectorParameters>(params);
                simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
                // model updates the UI
                model.setTabs(TestImages::getInstance().getTabs());

                // throttle thread to 20 ms
                std::this_thread::sleep_for(timer.remainingTimeAsDuration());
            }
        });
        break;
    }
    QObject::connect(&app, &QGuiApplication::aboutToQuit, [&thread, &curr_params, &source]() {
        thread.request_stop();
        source.requestStop();
        // small hack; change curr_params, so we can make the image thread check the stop_token again
        cv::aruco::DetectorParameters impossible_params;
        impossible_params.adaptiveThreshWinSizeMax = 1;
        impossible_params.adaptiveThreshWinSizeMin = 3;
        curr_params = impossible_params;
        curr_params.notify_all();
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("ImageModel", &model);
    engine.rootContext()->setContextProperty("_aruco", &arucoController);
    engine.load(QUrl("qrc:/stepviewer/src/main.qml"));
    return app.exec();
}
