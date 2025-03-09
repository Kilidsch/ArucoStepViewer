#include "sceneview.h"

#include "testimages.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/fl_draw.H>
#include <FL/platform.H>
#include <iostream>
SceneView::SceneView(int x, int y, int w, int h) : Fl_Tabs(x, y, w, h)
{
    auto &test_images = TestImages::getInstance();
    m_tabs = test_images.getTabs();

    this->handle_overflow(Fl_Tabs::OVERFLOW_PULLDOWN);

    fl_open_display();
    int ww = 0, hh = 0; // initialize to zero before calling fl_measure()
    fl_measure("test text", ww, hh);

    for (const auto &tab : m_tabs)
    {
        int rx, ry, rw, rh;
        this->client_area(rx, ry, rw, rh, hh);
        auto *group = new Fl_Group(rx, ry, rw, rh, "Test");
        group->copy_label(tab.name.c_str());
        auto *pic_box = new Fl_Box(0, 0, rw, rh);
        auto *img = new Fl_RGB_Image(tab.imgs.back().data, tab.imgs.back().cols, tab.imgs.back().rows,
                                     tab.imgs.back().channels(), static_cast<int>(tab.imgs.back().step));
        pic_box->image(img);
        group->end();
        group->resizable(pic_box);
        this->resizable(group); // overwrites all the time, but we dont care
    }
    this->end();
}
