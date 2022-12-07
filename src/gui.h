#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <opencv2/aruco.hpp>

class ParameterWindow
{
public:
    ParameterWindow(int argc, char **argv, void fun(Fl_Widget *, void *));

    cv::aruco::DetectorParameters getArucoParameters();

private:
    Fl_Window *window;
};

class ImageWindow
{
public:
    ImageWindow(int argc, char **argv, cv::Mat img1, cv::Mat img2);
};