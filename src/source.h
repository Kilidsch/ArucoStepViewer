#ifndef SOURCE_H
#define SOURCE_H

#include <mutex>
#include <opencv2/opencv.hpp>
#include <shared_mutex>
#include <thread>
#include <memory>
#include <chrono>

enum class InputType
{
    Image,
    VideoStream,
    Video
};

class Source
{
  public:
    virtual cv::Mat getImg() = 0;
    virtual void requestStop(){
        // only needed in implementations with worker threads
    };
};

std::unique_ptr<Source> createSource(InputType type, std::string path);

class ImageSource: public Source{
  private:
    cv::Mat m_img;
    cv::VideoCapture m_capture;

  public:
    ImageSource(const std::string& path);
    cv::Mat getImg() override;
};


class VideoStreamSource : public Source{
  private:
    cv::Mat m_img;
    cv::VideoCapture m_capture;
    std::jthread m_pull_thread;
    std::shared_mutex m_mut;

    void pullImage();

  public:
    VideoStreamSource(const std::string &path);
    void requestStop() override;
    cv::Mat getImg() override;
};

class VideoSource : public Source{
  private:
    cv::Mat m_img;
    cv::VideoCapture m_capture;
    std::chrono::time_point<std::chrono::steady_clock> m_last_call;
    std::chrono::seconds m_sec_per_frame;

  public:
    VideoSource(const std::string& path);
    cv::Mat getImg() override;
};


#endif // SOURCE_H
