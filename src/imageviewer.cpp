#include "imageviewer.h"
#include <FL/Fl_Box.H>

#include <opencv2/imgproc.hpp>
#include <print>

ImageViewer::ImageViewer(int x, int y, int w, int h, const char *label) : Fl_Group(x, y, w, h)
{
    copy_label(label);
    // needs to be inset: https://github.com/fltk/fltk/issues/1175#issuecomment-2578163418
    m_scroll = new Fl_Scroll(x + 5, y + 5, w - (2 * 5), h - (2 * 5));
    m_scroll->box(FL_NO_BOX);
    m_pic_box = new Fl_Box(x + 5, y + 5, w - 5, h - 5);
    m_scroll->end();
    this->end();
    this->resizable(m_scroll);
}

void ImageViewer::setImage(cv::Mat new_img)
{
    m_img = new_img; // shallow copy, but should be fine
    updateImage();
}

void ImageViewer::updateImage(int x_diff, int y_diff)
{
    // scale
    cv::resize(m_img, m_show_img, cv::Size{}, m_zoom, m_zoom);

    auto *old = m_pic_box->image();
    if (old)
    {
        delete old;
    }

    auto *new_img = new Fl_RGB_Image(m_show_img.data, m_show_img.cols, m_show_img.rows, m_show_img.channels(),
                                     static_cast<int>(m_show_img.step));
    m_pic_box->image(new_img);
    m_pic_box->size(m_show_img.cols, m_show_img.rows);
    m_scroll->scroll_to(m_scroll->xposition() + x_diff, m_scroll->yposition() + y_diff);
    redraw();
}

int ImageViewer::handle(int event)
{
    switch (event)
    {
    case FL_MOUSEWHEEL:
        scroll(Fl::event_dy(), Fl::event_x() - m_pic_box->x(), Fl::event_y() - m_pic_box->y());
        // std::cout << "Mousewheel event" << std::endl;
        return 1;
    default:
        return Fl_Group::handle(event);
    }
}

void ImageViewer::scroll(int dy, int x, int y)
{
    constexpr double float_inc = 0.02;
    if (dy < 0)
    {
        // scroll up, zoom in
        m_zoom = m_zoom - float_inc;
        int dx = static_cast<int>(-m_img.cols * float_inc / 2.);
        int dy = static_cast<int>(-m_img.rows * float_inc / 2.);
        updateImage(dx, dy);
    }
    else if (dy > 0)
    {
        m_zoom = m_zoom + float_inc;
        int dx = static_cast<int>(m_img.cols * float_inc / 2.);
        int dy = static_cast<int>(m_img.rows * float_inc / 2.);
        updateImage(dx, dy);
    }
}
