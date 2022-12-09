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

ImageWindow::ImageWindow(int argc, char **argv, const std::vector<Tab> &tabs)
{
    const int w = tabs.back().imgs.back().cols;
    const int h = tabs.back().imgs.back().rows;
    auto *imgWindow = new Fl_Window(w + 30, h + 140, "Image");
    {
        auto *tabs_widget = new Fl_Tabs(0, 0, w + 30, h + 130, "ImageTabs");
        {
            for (auto &[name, img_vec] : tabs)
            {
                auto *g = new Fl_Group(10, 30, w, h + 100, name);
                {
                    auto *pic_box = new Fl_Box(10, 30, w, h + 20, name);
                    cbHelpers.push_back(CallbackHelper{name, img_vec, pic_box});
                    auto &cbHelper = cbHelpers.back();
                    auto &img = cbHelper.images.front();
                    auto *image = new Fl_RGB_Image(img.data, img.cols, img.rows, img.channels(), img.step);
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
        tabs_widget->end();
    }
    imgWindow->end();
    imgWindow->show(argc, argv);
}

void ImageWindow::changeImage(Fl_Widget *w, void *data)
{
    auto *cbHelper = (CallbackHelper *)data;
    auto *slider = (Fl_Value_Slider *)w;
    int selectedScale = slider->value();

    auto &img = cbHelper->images.at(selectedScale);
    auto *image = new Fl_RGB_Image(img.data, img.cols, img.rows, 1);
    cbHelper->imgBox->image(image);
    cbHelper->imgBox->redraw();
}