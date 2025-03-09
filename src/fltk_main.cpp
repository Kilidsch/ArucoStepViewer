#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

#include "aruco.h"
#include "fltk_parameters.h"
#include "sceneview.h"
#include "source.h"
#include <FL/fl_draw.H>
#include <iostream>

std::jthread create_compute_thread(std::unique_ptr<Source> &source, InputType type, SceneView *view,
                                   std::atomic<cv::aruco::DetectorParameters> &curr_params)
{
    std::jthread computeThread;

    switch (type)
    {
    case InputType::Image:
        // When using an image, rerun on every change of parameters
        computeThread = std::jthread([&](std::stop_token stoken) {
            auto img = source->getImg();
            std::vector<int> markerIds;
            std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
            cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
            cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
            while (!stoken.stop_requested())
            {
                // re-run with new parameters; images are reset in simulateDetectMarkers
                auto params = curr_params.load();
                auto parameters = std::make_shared<cv::aruco::DetectorParameters>(params);
                simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
                // model updates the UI
                Fl::lock();
                view->setTabs(TestImages::getInstance().getTabs());
                Fl::unlock();
                Fl::awake();
                curr_params.wait(params);
            }
        });
        break;
    case InputType::VideoStream:
        [[fallthrough]];
    case InputType::Video:
        throw std::logic_error("Not implemented yet");
        break;
    }

    return computeThread;
}

int main(int argc, char **argv)
{
    Fl::lock();

    std::unique_ptr<Source> source = createSource(InputType::Image, R"(D:\KleineProjekte\ArucoStepViewer\test.png)");
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    auto img = source->getImg();
    simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

    std::atomic<cv::aruco::DetectorParameters> curr_params;
    auto callback = [&](cv::aruco::DetectorParameters params) {
        curr_params.store(params);
        curr_params.notify_all();
    };

    Fl_Window *window = new Fl_Window(800, 800);
    Fl_Flex *flex = new Fl_Flex(window->w(), window->h(), Fl_Flex::HORIZONTAL);
    auto *params = new Parameters(0, 0, 10, 10, callback);
    auto *scene_view = new SceneView(0, 0, 800, 800);
    flex->end();
    int ww = 0, hh = 0;
    fl_measure("Min. marker dist. rate: XXXXXXXXX", ww, hh);
    flex->fixed(params, ww);
    window->end();

    std::jthread computeThread = create_compute_thread(source, InputType::Image, scene_view, curr_params);
    window->resizable(window);
    //  window->size_range(params->w(), params->h());

    window->show(argc, argv);
    Fl::run();

    computeThread.request_stop();
    source->requestStop();
    // small hack; change curr_params, so we can make the image thread check the stop_token again
    cv::aruco::DetectorParameters impossible_params;
    impossible_params.adaptiveThreshWinSizeMax = 1;
    impossible_params.adaptiveThreshWinSizeMin = 3;
    curr_params = impossible_params;
    curr_params.notify_all();

    return 0;
}
