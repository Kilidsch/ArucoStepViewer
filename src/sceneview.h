#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <FL/Fl_Tabs.H>

#include <testimages.h>

class Fl_Box;

class SceneView : public Fl_Tabs
{
  public:
    SceneView(int x, int y, int w, int h);

  private:
    std::vector<Tab> m_tabs;
};

class ImageStack : public Fl_Tabs
{
    Tab m_tab;
    std::vector<Fl_Group *> m_groups;

  public:
    ImageStack(Tab &&initial_tab, int x, int y, int w, int h);

    void setTab(Tab &&tab);
};

#endif // SCENEVIEW_H
