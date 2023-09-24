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
    std::mutex m_mutex;

  public:
    static TestImages &getInstance()
    {
        static TestImages instance;
        return instance;
    }

    TestImages(const TestImages &) = delete;
    TestImages &operator=(const TestImages &) = delete;

    void addImg(const std::string &tab_name, const cv::Mat &img)
    {
        std::unique_lock lock(m_mutex);
        // small number of tabs, linear search should be ideal
        // (given tab names aren't huge)
        auto it = std::find_if(m_tabs.begin(), m_tabs.end(),
                               [tab_name](const Tab &tab) { return tab_name.compare(tab.name) == 0; });

        if (it == m_tabs.end())
        {
            m_tabs.push_back({tab_name, std::vector<cv::Mat>{img.clone()}});
        }
        else
        {
            it->imgs.push_back(img.clone());
        }
    }

    void clearTabs()
    {
        std::unique_lock lock(m_mutex);
        m_tabs.clear();
    }

    const std::vector<Tab> &getTabs() const
    {
        return m_tabs;
    }

  private:
    TestImages();
};

#endif // TESTIMAGES_H
