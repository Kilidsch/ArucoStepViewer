#ifndef SOURCE_H
#define SOURCE_H

#include <mutex>
#include <opencv2/opencv.hpp>
#include <shared_mutex>
#include <thread>

enum class InputType
{
    Image,
    Video
};

class Source
{
    InputType m_type;
    cv::VideoCapture m_capture;
    cv::Mat m_img;
    std::jthread m_pull_thread;
    std::shared_mutex m_mut;

  public:
    Source(InputType type, std::string path);

    void pullImage();
    void requestStop();

    cv::Mat getImg();
};

#endif // SOURCE_H
