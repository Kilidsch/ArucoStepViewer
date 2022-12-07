#include "gui.h"

#include <FL/Fl_Pack.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Input.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Tabs.H>

ParameterWindow::ParameterWindow(int argc, char **argv, void fun(Fl_Widget*,void*))
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

ImageWindow::ImageWindow(int argc, char **argv, cv::Mat img1, cv::Mat img2)
{
    
    auto *imgWindow = new Fl_Window(img1.cols + 30, img1.rows + 60, "Image");
    {
        auto *tabs = new Fl_Tabs(0, 0, img1.cols + 20, img1.rows + 50, "ImageTabs");
        {
            auto *g1 = new Fl_Group(10, 30, img1.cols, img1.rows, "Tab1");
            {
                auto *image = new Fl_RGB_Image(img1.data, img1.cols, img1.rows);
                auto *pic_box = new Fl_Box(10, 30, img1.cols, img1.rows + 20, "MeinBild");
                pic_box->image(image);
            }
            g1->end();

            auto *g2 = new Fl_Group(10, 30, img2.cols, img2.rows, "Tab2");
            {
                auto *image_rgb = new Fl_RGB_Image(img2.data, img2.cols, img2.rows);
                auto *pic_box2 = new Fl_Box(10, 30, img2.cols, img2.rows + 20, "MeinBild");
                pic_box2->image(image_rgb);
            }
            g2->end();
            tabs->resizable(g1);
        }
        tabs->end();
    }
    imgWindow->end();
    imgWindow->show(argc, argv);

}
