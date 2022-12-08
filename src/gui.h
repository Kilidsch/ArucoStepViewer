#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <opencv2/aruco.hpp>

struct Tab{
    const char * name;
    cv::Mat img;
};

class Fl_Box;

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
    ImageWindow(int argc, char **argv, std::vector<Tab> tabs, std::vector<cv::Mat>& threshImgs);
private:
    static void changeImage(Fl_Widget* w, void*);

    std::vector<cv::Mat> threshImgs;
    Fl_Box* threshBox;
};