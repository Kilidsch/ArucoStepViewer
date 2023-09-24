#include "gui.h"

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>

#include <opencv2/imgproc.hpp>

const cv::Size DEFAULT_SIZE{1280, 720};

ParameterWindow::ParameterWindow(int argc, char **argv, void fun(Fl_Widget *, void *), SimulateCallbackData &data)
{
    Fl::visual(FL_RGB);
    data.pmWin = this;
    window = new Fl_Window(340, 300);
    {
        Fl_Pack *pack = new Fl_Pack(100, 0, 240, 180, "PackLayout");
        {
            maxThresh = new Fl_Value_Input(120, 40, 100, 30, "Max. Thresh:");
            maxThresh->labeltype(FL_NORMAL_LABEL);
            maxThresh->minimum(3);
            // maxThresh->callback(fun, &data);

            minThresh = new Fl_Value_Input(120, 40, 100, 30, "Min. Thresh:");
            minThresh->labeltype(FL_NORMAL_LABEL);
            minThresh->minimum(3);
            // minThresh->callback(fun, &data);

            stepThresh = new Fl_Value_Input(120, 40, 100, 30, "Step Thresh:");
            stepThresh->labeltype(FL_NORMAL_LABEL);
            // stepThresh->callback(fun, &data);

            constThresh = new Fl_Value_Input(120, 40, 100, 30, "Thrsh constant:");
            minPerimeter = new Fl_Value_Input(120, 40, 100, 30, "Min perimeter");
            maxPerimeter = new Fl_Value_Input(120, 40, 100, 30, "Max perimeter");
            minCornerDist = new Fl_Value_Input(120, 40, 100, 30, "Min corner dist.");
            accRate = new Fl_Value_Input(120, 40, 100, 30, "Poly. Acc.");
            minMarkerDist = new Fl_Value_Input(120, 40, 100, 30, "Min Marker dist.");

            auto *btn = new Fl_Button(120, 40, 100, 30, "Run");
            btn->when(FL_WHEN_RELEASE);
            btn->callback(fun, &data);
        }
        pack->end();
    }
    window->end();

    fromArucoParameters(cv::aruco::DetectorParameters::create());
    window->show(argc, argv);
}

cv::Ptr<cv::aruco::DetectorParameters> ParameterWindow::getArucoParameters()
{
    auto params = cv::aruco::DetectorParameters::create();
    params->adaptiveThreshWinSizeMax = (int)maxThresh->value();
    params->adaptiveThreshWinSizeMin = (int)minThresh->value();
    params->adaptiveThreshWinSizeStep = (int)stepThresh->value();
    params->adaptiveThreshConstant = (int)constThresh->value();

    params->minMarkerPerimeterRate = minPerimeter->value();
    params->maxMarkerPerimeterRate = maxPerimeter->value();
    params->minCornerDistanceRate = minCornerDist->value();
    params->polygonalApproxAccuracyRate = accRate->value();

    params->minMarkerDistanceRate = minMarkerDist->value();

    return params;
}

void ParameterWindow::fromArucoParameters(cv::Ptr<cv::aruco::DetectorParameters> params)
{
    maxThresh->value(params->adaptiveThreshWinSizeMax);
    minThresh->value(params->adaptiveThreshWinSizeMin);
    stepThresh->value(params->adaptiveThreshWinSizeStep);
    constThresh->value(params->adaptiveThreshConstant);

    minPerimeter->value(params->minMarkerPerimeterRate);
    maxPerimeter->value(params->maxMarkerPerimeterRate);
    minCornerDist->value(params->minCornerDistanceRate);
    accRate->value(params->polygonalApproxAccuracyRate);

    minMarkerDist->value(params->minMarkerDistanceRate);
}

static void resizeImages(std::vector<cv::Mat> &imgs, cv::Size s = DEFAULT_SIZE)
{
    for (auto &img : imgs)
    {
        cv::resize(img, img, s);
    }
}

ImageWindow::ImageWindow(int argc, char **argv, const std::vector<Tab> &tabs)
{
    const int w = DEFAULT_SIZE.width;  // tabs.back().imgs.back().cols;
    const int h = DEFAULT_SIZE.height; // tabs.back().imgs.back().rows;
    imgWindow = new Fl_Window(w + 30, h + 140, "Image");
    init(tabs);
    imgWindow->show(argc, argv);
}

void ImageWindow::init(const std::vector<Tab> &tabs)
{
    const int w = DEFAULT_SIZE.width;  // tabs.back().imgs.back().cols;
    const int h = DEFAULT_SIZE.height; // tabs.back().imgs.back().rows;

    if (tabsWidget != nullptr)
    {
        Fl::delete_widget(tabsWidget);
    }

    imgWindow->begin();
    {
        tabsWidget = new Fl_Tabs(0, 0, w + 30, h + 130, "ImageTabs");
        {
            for (auto &[name, img_vec] : tabs)
            {
                auto *g = new Fl_Group(10, 30, w, h + 100, name.c_str());
                {
                    auto *pic_box = new Fl_Box(10, 30, w, h + 20, name.c_str());
                    cbHelpers.push_back(CallbackHelper{name, img_vec, pic_box});
                    auto &cbHelper = cbHelpers.back();
                    resizeImages(cbHelper.images);
                    auto &img = cbHelper.images.front();
                    auto *image =
                        new Fl_RGB_Image(img.data, img.cols, img.rows, img.channels(), static_cast<int>(img.step));
                    pic_box->image(image);

                    auto *scale_selection = new Fl_Value_Slider(10, img.rows + 60, img.cols, 30, "Scale:");
                    scale_selection->maximum(img_vec.size() - 1.);
                    scale_selection->minimum(0);
                    scale_selection->step(1);
                    scale_selection->type(FL_HOR_SLIDER);
                    scale_selection->value(0);
                    scale_selection->callback(&ImageWindow::changeImage, &cbHelper);
                }
                g->end();
            }
        }
        tabsWidget->end();
    }
    imgWindow->end();
}

void ImageWindow::changeImage(Fl_Widget *w, void *data)
{
    auto *cbHelper = (CallbackHelper *)data;
    auto *slider = (Fl_Value_Slider *)w;
    int selectedScale = static_cast<int>(slider->value());

    auto &img = cbHelper->images.at(selectedScale);
    auto *image = new Fl_RGB_Image(img.data, img.cols, img.rows, img.channels(), static_cast<int>(img.step));
    cbHelper->imgBox->image(image);
    cbHelper->imgBox->redraw();
}
