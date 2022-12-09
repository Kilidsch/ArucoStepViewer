#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <opencv2/aruco.hpp>
#include <vector>
#include <list>
#include <optional>

class Fl_Box;

struct CallbackHelper
{
    std::string name;
    std::vector<cv::Mat> images;
    Fl_Box *imgBox;
};

struct Tab
{
    const char *name;
    std::vector<cv::Mat> imgs;
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
    ImageWindow(int argc, char **argv, const std::vector<Tab>& tabs);

private:
    static void changeImage(Fl_Widget *w, void *self);

    // using list for reference stability reasons
    std::list<CallbackHelper> cbHelpers;
};