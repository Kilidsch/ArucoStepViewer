#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <opencv2/aruco.hpp>
#include <vector>
#include <list>
#include <optional>

class Fl_Box;
class Fl_Tabs;
class Fl_Value_Input;
class ImageWindow;
class ParameterWindow;

struct SimulateCallbackData{
    cv::Mat img;
    ImageWindow& win;
    ParameterWindow *pmWin = nullptr;
};

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
    ParameterWindow(int argc, char **argv, void fun(Fl_Widget *, void *), SimulateCallbackData& data);

    cv::Ptr<cv::aruco::DetectorParameters> getArucoParameters();
    void fromArucoParameters(cv::Ptr<cv::aruco::DetectorParameters> params);

private:
    Fl_Window *window;
    Fl_Value_Input *maxThresh;
    Fl_Value_Input *minThresh;
    Fl_Value_Input *stepThresh;
    Fl_Value_Input *constThresh;
};

class ImageWindow
{
public:
    ImageWindow(int argc, char **argv, const std::vector<Tab> &tabs);
    void init(const std::vector<Tab> &tabs);

private:
    static void changeImage(Fl_Widget *w, void *self);

    // using list for reference stability reasons
    std::list<CallbackHelper> cbHelpers;
    Fl_Window *imgWindow;
    Fl_Tabs *tabsWidget = nullptr;
};