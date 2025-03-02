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
    // NOTE:The widgets contained in each child should leave some clear space (five pixels as of FLTK 1.4.x) at the top
    // or bottom of the group (where the tabs are displayed). Otherwise drawing the children may interfere with the
    // selection border between the tabs and the children. This is particularly important if the child group is an
    // Fl_Scroll widget: either the Fl_Scroll widget must be inset by five pixels relative to other children or it can
    // be wrapped inside another Fl_Group and inset by five pixels within this group so the contents of the Fl_Scroll
    // widget are kept away from the tabs by this amount.

    // Want to create this automagically
    auto &test_images = TestImages::getInstance();
    m_tabs = test_images.getTabs();

    this->handle_overflow(Fl_Tabs::OVERFLOW_PULLDOWN);

    fl_open_display();
    int ww = 0, hh = 0; // initialize to zero before calling fl_measure()
    fl_measure("test text", ww, hh);

    // Fl_Flex *flex = new Fl_Flex(w, h, Fl_Flex::VERTICAL);
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
        std::cout << "Tab: " << tab.name << std::endl;
        group->end();
        group->resizable(pic_box);
        this->resizable(group); // overwrites all the time, but we dont care
    }
    // flex->end();
    this->end();
}
