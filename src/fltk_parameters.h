#ifndef FLTK_PARAMETERS_H
#define FLTK_PARAMETERS_H

#include <FL/Fl_Flex.H>
#include <FL/Fl_Grid.H>
#include <opencv2/objdetect/aruco_detector.hpp>

class Parameters : public Fl_Flex
{
  public:
    Parameters(int x, int y, int w, int h,
               std::function<void(cv::aruco::DetectorParameters, cv::aruco::PredefinedDictionaryType)> cb);

  private:
    cv::aruco::DetectorParameters m_params;
};

#endif // FLTK_PARAMETERS_H
