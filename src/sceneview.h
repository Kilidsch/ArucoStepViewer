#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <FL/Fl_Tabs.H>

#include <testimages.h>

class SceneView : public Fl_Tabs
{
  public:
    SceneView(int x, int y, int w, int h);

  private:
    std::vector<Tab> m_tabs;
};

#endif // SCENEVIEW_H
