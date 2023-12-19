#ifndef TESTIMAGES_H
#define TESTIMAGES_H

#include <mutex>
#include <opencv2/core.hpp>
#include <vector>

struct Tab
{
    std::string name;
    std::vector<cv::Mat> imgs;
};

class TestImages
{
  private:
    std::vector<Tab> m_tabs;
    mutable std::mutex m_mutex;

  public:
    static TestImages &getInstance()
    {
        static TestImages instance;
        return instance;
    }

    TestImages(const TestImages &) = delete;
    TestImages &operator=(const TestImages &) = delete;

    void addImg(const std::string &tab_name, const cv::Mat &img);
    void clearTabs();
    const std::vector<Tab> &getTabs() const;

  private:
    TestImages() = default;
};

#endif // TESTIMAGES_H
