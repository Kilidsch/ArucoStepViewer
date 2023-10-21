#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "aruco.h" // for simulateDetectMarkers
#include "arucoparamscontroller.h"
#include "imagemodel.h"

#include <QCommandLineParser>
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

enum class InputType
{
    Image,
    Video
};

class Source
{
    InputType m_type;
    cv::VideoCapture m_capture;
    cv::Mat m_img;

  public:
    Source(InputType type, std::string path) : m_type(type)
    {

        switch (m_type)
        {
        case InputType::Image:
            m_img = cv::imread(path);
            if (m_img.empty())
            {
                std::stringstream ss;
                ss << "Could not open image (" << path << ")!";
                throw std::runtime_error(ss.str());
            }
            break;
        case InputType::Video:
            // special support for "0" as 0, i.e. first camera
            if (path == "0")
            {

                m_capture = cv::VideoCapture(0);
            }
            else
            {
                m_capture = cv::VideoCapture(path);
            }
            if (!m_capture.isOpened())
            {
                std::stringstream ss;
                ss << "Could not open video(stream) (" << path << ")!";
                throw std::runtime_error(ss.str());
            }
            break;
        }
    }

    cv::Mat getImg()
    {
        std::this_thread::sleep_for(10ms);
        switch (m_type)
        {
        case InputType::Image:
            return m_img.clone();

        case InputType::Video:
            // TODO: add throttling for video-files (camera stream does not need it)
            m_capture.read(m_img);
            if (m_img.empty())
            {
                throw std::runtime_error("Could not read new frame from video(stream)");
            }
            return m_img.clone();
        }
        throw std::logic_error("getImg did not implement used source");
    }
};

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
        // TODO: kill thread (std::stop_token?) when window is closed
        thread = std::jthread([&](std::stop_token stoken) {
            while (!stoken.stop_requested())
            {
                // getImg is a blocking call and limits the speed of this thread
                // no manual throttling is done
                auto img = source.getImg();
                auto params = curr_params.load();
                auto parameters = std::make_shared<cv::aruco::DetectorParameters>(params);
                simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
                // model updates the UI
                model.setTabs(TestImages::getInstance().getTabs());
            }
        });
        break;
    }
    QObject::connect(&app, &QGuiApplication::aboutToQuit, [&thread, &curr_params]() {
        thread.request_stop();
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
