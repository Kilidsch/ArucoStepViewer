#include "sceneview.h"

#include "imageviewer.h"
#include "testimages.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/fl_draw.H>
#include <FL/platform.H>

SceneView::SceneView(int x, int y, int w, int h) : Fl_Tabs(x, y, w, h)
{
    auto &test_images = TestImages::getInstance();
    m_tabs = test_images.getTabs();

    this->handle_overflow(Fl_Tabs::OVERFLOW_PULLDOWN);

    fl_open_display();
    int ww = 0, hh = 0; // initialize to zero before calling fl_measure()
    fl_measure("test text", ww, hh);

    for (auto &tab : m_tabs)
    {
        int rx, ry, rw, rh;
        this->client_area(rx, ry, rw, rh, hh);
        auto stack = new ImageStack(std::move(tab), rx, ry, rw, rh);
        this->resizable(stack);
    }
    this->end();
}

void SceneView::setTabs(std::vector<Tab> tabs)
{
    for (size_t i = 0; i < tabs.size(); ++i)
    {
        dynamic_cast<ImageStack *>(child(static_cast<int>(i)))->setTab(std::move(tabs[i]));
    }
}

ImageStack::ImageStack(Tab &&initial_tab, int x, int y, int w, int h) : Fl_Tabs(x, y, w, h)
{
    m_tab = std::move(initial_tab);
    this->copy_label(m_tab.name.c_str());
    this->handle_overflow(Fl_Tabs::OVERFLOW_PULLDOWN);
    int ww = 0, hh = 0; // initialize to zero before calling fl_measure()
    fl_measure("test text", ww, hh);

    m_groups.resize(m_tab.imgs.size());
    for (size_t i = 0; i < m_tab.imgs.size(); ++i)
    {
        int rx, ry, rw, rh;
        this->client_area(rx, ry, rw, rh, hh);
        m_groups[i] = new ImageViewer(rx, ry, rw, rh, std::to_string(i).c_str());
        m_groups[i]->setImage(m_tab.imgs[i]);
    }
    this->end();
}

void ImageStack::setTab(Tab &&tab)
{
    if (tab.imgs.size() < m_groups.size())
    {
        for (size_t i = tab.imgs.size(); i < m_groups.size(); i++)
        {
            // since FLTK 1.3 schedules removal from parent as well
            delete m_groups[i];
        }
        // NOTE: need to make a still existing group resizable?
        m_groups.resize(tab.imgs.size());
    }
    else if (tab.imgs.size() > m_groups.size())
    {
        this->begin();
        for (size_t i = m_groups.size(); i < tab.imgs.size(); ++i)
        {
            auto rx = m_groups.front()->x();
            auto ry = m_groups.front()->y();
            auto rw = m_groups.front()->w();
            auto rh = m_groups.front()->h();
            auto *group = new ImageViewer(rx, ry, rw, rh, std::to_string(i).c_str());
            m_groups.push_back(group);
        }
        this->end();
    }

    for (size_t i = 0; i < m_groups.size(); i++)
    {
        m_groups[i]->setImage(tab.imgs[i]);
    }
    // delete old stuff after changing all images (not strictly needed, since we lock ui, but still)
    m_tab = std::move(tab);
}
