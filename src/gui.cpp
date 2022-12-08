#include "gui.h"

#include <FL/Fl_Pack.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Input.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Tabs.H>


ParameterWindow::ParameterWindow(int argc, char **argv, void fun(Fl_Widget *, void *))
{
    Fl::visual(FL_RGB);
    window = new Fl_Window(340, 300);
    {
        Fl_Pack *pack = new Fl_Pack(100, 0, 240, 180, "PackLayout");
        {
            Fl_Value_Input *input = new Fl_Value_Input(120, 40, 100, 30, "Max. Thresh:");
            input->labeltype(FL_NORMAL_LABEL);
            input->callback(fun);

            Fl_Value_Input *inp2 = new Fl_Value_Input(120, 40, 100, 30, "Min. Thresh:");
            inp2->labeltype(FL_NORMAL_LABEL);
            inp2->callback(fun);
        }
        pack->end();
    }
    window->end();
    window->show(argc, argv);
}

ImageWindow::ImageWindow(int argc, char **argv, std::vector<Tab> tabs, std::vector<cv::Mat> &threshImgsIn)
{
    threshImgs.reserve(threshImgsIn.size());
    for (size_t i = 0; i < threshImgsIn.size(); ++i)
    {
        threshImgs.push_back(threshImgsIn[i].clone());
    }

    const int w = tabs.back().img.cols;
    const int h = tabs.back().img.rows;
    auto *imgWindow = new Fl_Window(w + 30, h + 120, "Image");
    {
        auto *tabs_widget = new Fl_Tabs(0, 0, w + 20, h + 120, "ImageTabs");
        {
            for (auto &[name, img] : tabs)
            {
                auto *g = new Fl_Group(10, 30, img.cols, img.rows, name);
                {
                    auto *image = new Fl_RGB_Image(img.data, img.cols, img.rows);
                    auto *pic_box = new Fl_Box(10, 30, img.cols, img.rows + 20, name);
                    pic_box->image(image);
                }
                g->end();
            }
            auto &img = threshImgs.front();
            auto *g = new Fl_Group(10, 30, img.cols, img.rows + 100, "Threshold");
            {
                auto *image = new Fl_RGB_Image(img.data, img.cols, img.rows, img.channels(), img.step);
                threshBox = new Fl_Box(10, 30, img.cols, img.rows + 20, "Threshold");
                threshBox->image(image);

                auto *scale_selection = new Fl_Value_Slider(10, img.rows + 60, img.cols, 30, "Scale:");
                scale_selection->maximum(threshImgs.size() - 1.);
                scale_selection->minimum(0);
                scale_selection->step(1);
                scale_selection->type(FL_HOR_SLIDER);
                scale_selection->value(0);
                scale_selection->callback(&ImageWindow::changeImage, (void *)this);
            }
            g->end();
        }
        tabs_widget->end();
    }
    imgWindow->end();
    imgWindow->show(argc, argv);
}

void ImageWindow::changeImage(Fl_Widget *w, void *data)
{
    ImageWindow *imgWindow = (ImageWindow *)data;
    Fl_Value_Slider *slider = (Fl_Value_Slider *)w;
    int selectedScale = slider->value();

    auto &img = imgWindow->threshImgs.at(selectedScale);
    auto *image = new Fl_RGB_Image(img.data, img.cols, img.rows, 1);
    imgWindow->threshBox->image(image);
    imgWindow->threshBox->redraw();
}