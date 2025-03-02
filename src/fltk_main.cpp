#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

#include "aruco.h"
#include "fltk_parameters.h"
#include "sceneview.h"
#include "source.h"
#include <iostream>

int main(int argc, char **argv)
{
    std::unique_ptr<Source> source = createSource(InputType::Image, R"(D:\KleineProjekte\ArucoStepViewer\test.png)");
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    auto img = source->getImg();
    simulateDetectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

    Fl_Window *window = new Fl_Window(800, 800);
    Fl_Flex *flex = new Fl_Flex(window->w(), window->h(), Fl_Flex::HORIZONTAL);
    auto *params = new Parameters(0, 0, 10, 10);
    auto *scene_view = new SceneView(0, 0, 800, 800);
    flex->end();
    window->end();

    window->resizable(window);
    //  window->size_range(params->w(), params->h());

    window->show(argc, argv);
    return Fl::run();
}
