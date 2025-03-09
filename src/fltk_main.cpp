#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

#include "aruco.h"
#include "fltk_parameters.h"
#include "ratetimer.h"
#include "sceneview.h"
#include "source.h"
#include <FL/fl_draw.H>
#include <argparse/argparse.hpp>
#include <iostream>

using namespace std::chrono_literals;

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
        // When video(stream), rerun on every new frame
        computeThread = std::jthread([&](std::stop_token stoken) {
            std::vector<int> markerIds;
            std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
            cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
            cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
            constexpr auto min_time = 20ms;
            RateTimer timer(min_time);
            while (!stoken.stop_requested())
            {
                auto img = source->getImg();
                auto params = curr_params.load();
                auto parameters = std::make_shared<cv::aruco::DetectorParameters>(params);
                simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
                // model updates the UI
                Fl::lock();
                if (stoken.stop_requested())
                {
                    // view was deleted in the meantime; would crash on setTabs
                    break;
                }
                view->setTabs(TestImages::getInstance().getTabs());
                Fl::unlock();
                Fl::awake();

                // throttle thread to 20 ms
                timer.sleep();
            }
        });
        break;
    }

    return computeThread;
}

struct Args
{
    std::string path;
    InputType type;
};

Args parse_args(int argc, char **argv)
{
    argparse::ArgumentParser program("Aruco Step Viewer");

    // when updating dependencies, could use this
    // auto &group = program.add_mutually_exclusive_group(true);

    program.add_argument("--image").help("path to image file");
    program.add_argument("--video").help("path to video file");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n\n" << program;
        std::exit(1);
    }

    bool video_given = program.is_used("--video");
    bool image_given = program.is_used("--image");
    if (video_given && image_given)
    {
        std::cerr << "Give either image or video; not both!";
        std::exit(42);
    }
    if (!video_given && !image_given)
    {
        std::cerr << "Need to give either image or video; none was given";
        std::exit(1337);
    }

    if (video_given)
    {
        return {program.get("--video"), InputType::Video};
    }
    else
    {
        return {program.get("--image"), InputType::Image};
    }
}

int main(int argc, char **argv)
{
    Fl::lock();

    auto args = parse_args(argc, argv);
    std::unique_ptr<Source> source = createSource(args.type, args.path);
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

    std::jthread computeThread = create_compute_thread(source, args.type, scene_view, curr_params);
    window->resizable(window);
    //  window->size_range(params->w(), params->h());

    window->show(0, argv);
    Fl::run();
    Fl::unlock();
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
