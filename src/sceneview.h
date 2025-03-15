#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <FL/Fl_Tabs.H>

#include <testimages.h>

class Fl_Box;
class ImageViewer;
class SceneView : public Fl_Tabs
{
  public:
    SceneView(int x, int y, int w, int h);
    void setTabs(std::vector<Tab> tabs);

  private:
    std::vector<Tab> m_tabs;
    std::vector<Fl_Tabs *> m_tab_widgets;
};

class ImageStack : public Fl_Tabs
{
    Tab m_tab;
    std::vector<ImageViewer *> m_groups;

  public:
    ImageStack(Tab &&initial_tab, int x, int y, int w, int h);

    void setTab(Tab &&tab);
};

#endif // SCENEVIEW_H
