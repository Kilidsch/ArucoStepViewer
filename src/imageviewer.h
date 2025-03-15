#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Image.H> // forward-declare?
#include <FL/Fl_Scroll.H>
#include <opencv2/core.hpp>
class Fl_Box;

class ImageViewer : public Fl_Group
{
  public:
    ImageViewer(int x, int y, int w, int h, const char *label);

    void setImage(cv::Mat new_img);

    void updateImage(int x_diff = 0, int y_diff = 0);
    int handle(int event) override;
    void scroll(int dy, int x, int y);

  private:
    Fl_Box *m_pic_box;
    Fl_Scroll *m_scroll;
    cv::Mat m_img;

    double m_zoom{1.0};
    cv::Point2f m_pos{0, 0};
    bool m_drag_enabled = false;
    int m_drag_x;
    int m_drag_y;
};

#endif // IMAGEVIEWER_H
