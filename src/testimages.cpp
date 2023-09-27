#include "testimages.h"

void TestImages::addImg(const std::string &tab_name, const cv::Mat &img)
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

void TestImages::clearTabs()
{
    std::unique_lock lock(m_mutex);
    m_tabs.clear();
}

const std::vector<Tab> &TestImages::getTabs() const
{
    return m_tabs;
}
