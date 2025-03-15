#include "imageviewer.h"
#include <FL/Fl_Box.H>

#include <FL/fl_draw.H>
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
    auto *old = m_pic_box->image();
    if (old)
    {
        delete old;
    }
    auto *new_img_fltk =
        new Fl_RGB_Image(m_img.data, m_img.cols, m_img.rows, m_img.channels(), static_cast<int>(m_img.step));
    m_pic_box->image(new_img_fltk);
    updateImage();
}

void ImageViewer::updateImage(int x_diff, int y_diff)
{
    auto *new_img = m_pic_box->image();
    new_img->scale(static_cast<int>(m_img.cols * m_zoom), static_cast<int>(m_img.rows * m_zoom), 1, 1);
    if (m_zoom > 1)
    {
        new_img->scaling_algorithm(FL_RGB_SCALING_NEAREST);
    }
    else
    {
        new_img->scaling_algorithm(FL_RGB_SCALING_BILINEAR);
    }

    m_pic_box->resize(m_pic_box->x(), m_pic_box->y(), static_cast<int>(m_img.cols * m_zoom),
                      static_cast<int>(m_img.rows * m_zoom));

    // keep pixel under mouse (if zoomed in with scrollbar)
    auto new_x_pos = m_scroll->xposition() + x_diff;
    new_x_pos = std::clamp<int>(new_x_pos, static_cast<int>(m_scroll->hscrollbar.minimum()),
                                static_cast<int>(m_scroll->hscrollbar.maximum()));
    auto new_y_pos = m_scroll->yposition() + y_diff;
    new_y_pos = std::clamp<int>(new_y_pos, static_cast<int>(m_scroll->scrollbar.minimum()),
                                static_cast<int>(m_scroll->scrollbar.maximum()));
    m_scroll->scroll_to(new_x_pos, new_y_pos);

    redraw();
}

int ImageViewer::handle(int event)
{
    switch (event)
    {
    case FL_MOUSEWHEEL:
        scroll(Fl::event_dy(), Fl::event_x() - m_pic_box->x(), Fl::event_y() - m_pic_box->y());
        return 1;
    case FL_PUSH:
        if (Fl_Group::handle(event))
        {
            return 1;
        }
        if (Fl::event_button() == FL_LEFT_MOUSE)
        {
            if (m_pic_box->w() > m_scroll->w() || m_pic_box->h() > m_scroll->h())
            {
                m_drag_enabled = true;
                Fl::grab(this->window());
                m_drag_x = m_scroll->xposition() + Fl::event_x();
                m_drag_y = m_scroll->yposition() + Fl::event_y();
                fl_cursor(FL_CURSOR_MOVE, FL_BLACK, FL_WHITE);
                return 1;
            }
        }
        break;
    case FL_RELEASE:
        if (Fl_Group::handle(event))
        {
            return 1;
        }
        if (Fl::event_button() == FL_LEFT_MOUSE)
        {
            if (Fl_Group::handle(event))
            {
                return 1;
            }
            if (m_drag_enabled)
            {
                Fl::grab(nullptr);

                fl_cursor(FL_CURSOR_DEFAULT, FL_BLACK, FL_WHITE);
                m_drag_enabled = false;
                return 1;
            }
        }
        break;
    case FL_DRAG:
        if (Fl_Group::handle(event))
        {
            return 1;
        }
        if (Fl::event_button() == FL_LEFT_MOUSE)
        {

            if (m_drag_enabled)
            {
                int x = m_drag_x - Fl::event_x();
                int y = m_drag_y - Fl::event_y();

                const int x_max = m_pic_box->w() - m_scroll->w() + m_scroll->scrollbar.w();
                const int y_max = m_pic_box->h() - m_scroll->h() + m_scroll->hscrollbar.h();

                x = std::clamp(x, 0, x_max);
                y = std::clamp(y, 0, y_max);
                m_scroll->scroll_to(x, y);
            }
            return 1;
        }
        break;
    }
    return Fl_Group::handle(event);
}

void ImageViewer::scroll(int dy, int x, int y)
{
    constexpr double zoom_inc = 1.1;
    if (dy < 0)
    {
        // scroll up, zoom in
        m_zoom *= zoom_inc;
        int dx = static_cast<int>(x * (zoom_inc - 1));
        int dy = static_cast<int>(y * (zoom_inc - 1));
        updateImage(dx, dy);
    }
    else if (dy > 0)
    {
        // scroll down, zoom out
        m_zoom *= 1 / zoom_inc;
        int dx = static_cast<int>(x * (1 / zoom_inc - 1));
        int dy = static_cast<int>(y * (1 / zoom_inc - 1));
        updateImage(dx, dy);
    }
}
