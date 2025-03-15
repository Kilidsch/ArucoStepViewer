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
    this->resizable(m_scroll); // TODO: keep the image centered
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
    std::println("BEFORE: xpos={} zpos={} x={} y={}", m_scroll->xposition(), m_scroll->yposition(), m_pic_box->x(),
                 m_pic_box->y());
    double xpos = m_scroll->xposition();
    double xmax = m_scroll->hscrollbar.maximum();
    xpos -= m_scroll->hscrollbar.minimum();
    double ypos = m_scroll->yposition();
    double ymax = m_scroll->scrollbar.maximum();
    ypos -= m_scroll->scrollbar.minimum();
    std::println("x% = {} y% = {}", xpos / xmax, ypos / ymax);
    double x_factor = std::isnan(xpos / xmax) ? 0 : xpos / xmax;
    double y_factor = std::isnan(ypos / ymax) ? 0 : ypos / ymax;
    m_pic_box->resize(m_pic_box->x(), m_pic_box->y(), m_show_img.cols, m_show_img.rows);
    // m_pic_box->size(m_show_img.cols, m_show_img.rows);
    std::println("AFTER:  xpos={} zpos={} x={} y={}", m_scroll->xposition(), m_scroll->yposition(), m_pic_box->x(),
                 m_pic_box->y());

    auto new_x_pos = m_scroll->xposition() + x_diff;
    new_x_pos = std::clamp<int>(new_x_pos, m_scroll->hscrollbar.minimum(), m_scroll->hscrollbar.maximum());
    auto new_y_pos = m_scroll->yposition() + y_diff;
    new_y_pos = std::clamp<int>(new_y_pos, m_scroll->scrollbar.minimum(), m_scroll->scrollbar.maximum());
    m_scroll->scroll_to(new_x_pos, new_y_pos);
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

                bool x_move = true;
                bool y_move = true;
                const int x_max = m_pic_box->w() - m_scroll->w() + m_scroll->scrollbar.w();
                const int y_max = m_pic_box->h() - m_scroll->h() + m_scroll->hscrollbar.h();

                if (x <= 0)
                {
                    x_move = false;
                    // m_drag_x -= x;
                    x = 0;
                }
                else if (x >= x_max)
                {
                    x_move = false;
                    // m_drag_x -= (x - x_max);
                    x = x_max;
                }

                if (y <= 0)
                {
                    y_move = false;
                    // m_drag_y -= y;
                    y = 0;
                }
                else if (y >= y_max)
                {
                    y_move = false;
                    // m_drag_y -= (y - y_max);
                    y = y_max;
                }

                if (x_move || y_move)
                {
                    m_scroll->scroll_to(x, y);
                }
            }
            return 1;
        }
        break;
    }
    return Fl_Group::handle(event);
}

void ImageViewer::scroll(int dy, int x, int y)
{
    constexpr double float_inc = 1.1;
    if (dy < 0)
    {
        // scroll up, zoom in
        double old_zoom = m_zoom;
        m_zoom *= float_inc;
        // m_zoom = m_zoom - float_inc;
        int dx = static_cast<int>(x * (float_inc - 1));
        int dy = static_cast<int>(y * (float_inc - 1));
        std::println("dx={} dy={}", dx, dy);
        updateImage(dx, dy);
    }
    else if (dy > 0)
    {
        double old_zoom = m_zoom;
        m_zoom *= 1 / float_inc;
        int dx = static_cast<int>(-x * (float_inc - 1));
        int dy = static_cast<int>(-y * (float_inc - 1));
        std::println("dx={} dy={}", dx, dy);
        updateImage(dx, dy);
    }
}
