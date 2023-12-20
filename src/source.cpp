#include "source.h"

#include <QDeadlineTimer>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

using namespace std::chrono_literals;

Source::Source(InputType type, std::string path) : m_type(type)
{

    // TODO: differentiate vs video-file and video-stream
    // one needs continous pulling, the other should just be read frame for frame
    switch (m_type)
    {
    case InputType::Image:
        m_img = cv::imread(path);
        if (m_img.empty())
        {
            std::stringstream ss;
            ss << "Could not open image (" << path << ")!";
            throw std::runtime_error(ss.str());
        }
        break;
    case InputType::Video:
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
        break;
    }
}

void Source::pullImage()
{
    // continously grab frames such that no buffer build up occurs for web streams
    std::unique_lock lock(m_mut);
    m_capture.grab();
}

void Source::requestStop()
{
    m_pull_thread.request_stop();
}

cv::Mat Source::getImg()
{
    switch (m_type)
    {
    case InputType::Image:
        return m_img.clone();

    case InputType::Video:
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
    throw std::logic_error("getImg did not implement used source");
}
