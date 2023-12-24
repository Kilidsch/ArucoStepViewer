#include "source.h"

#include <QDeadlineTimer>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <filesystem>

using namespace std::chrono_literals;


void VideoStreamSource::pullImage()
{
    // continously grab frames such that no buffer build up occurs for web streams
    std::unique_lock lock(m_mut);
    m_capture.grab();
}

VideoStreamSource::VideoStreamSource(const std::string &path)
{
  // special support for "0" as 0, i.e. first camera
    if (path == "0")
    {

        m_capture = cv::VideoCapture(0);
    }
    else
    {
        m_capture = cv::VideoCapture(path);
    }
    if (!m_capture.isOpened())
    {
        std::stringstream ss;
        ss << "Could not open video(stream) (" << path << ")!";
        throw std::runtime_error(ss.str());
    }

    m_pull_thread = std::jthread([this](std::stop_token st) {
        while (!st.stop_requested())
        {
            pullImage();
        }
    });
}

void VideoStreamSource::requestStop()
{
    m_pull_thread.request_stop();
}

cv::Mat VideoStreamSource::getImg()
{
    std::shared_lock lock(m_mut);

           // could be video stream and we wait on key-frame (I-frame)
    QDeadlineTimer timer(30s);
    bool success = false;
    do
    {
        success = m_capture.retrieve(m_img);
        if (!success)
        {
            m_capture.grab();
        }
    } while (!timer.hasExpired() && !success);

    if (!success)
    {
        throw std::runtime_error("Could not read new frame from video(stream)");
    }

    return m_img.clone();
}


ImageSource::ImageSource(const std::string &path)
{
    m_img = cv::imread(path);
    if (m_img.empty())
    {
        std::stringstream ss;
        ss << "Could not open image (" << path << ")!";
        throw std::runtime_error(ss.str());
    }
}

cv::Mat ImageSource::getImg()
{
    return m_img.clone();
}

std::unique_ptr<Source> createSource(InputType type, std::string path)
{
    switch(type){
    case InputType::Image:
        return std::make_unique<ImageSource>(path);
    case InputType::VideoStream:
        return std::make_unique<VideoStreamSource>(path);
    case InputType::Video:
        return std::make_unique<VideoSource>(path);
    default:
        throw std::logic_error("Input type has no createSource");
    }
}

cv::Mat VideoSource::getImg()
{
    // TODO: implement!
    return m_img;
}
