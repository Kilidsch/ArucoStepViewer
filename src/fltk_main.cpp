#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

#include "fltk_parameters.h"
#include "fluid/parameters.h"
#include <iostream>

int main(int argc, char **argv)
{
    Fl_Window *window = new Fl_Window(800, 800);

    auto *params = new Parameters(0, 0, window->w(), window->h());
    window->end();

    window->resizable(params);
    //  window->size_range(params->w(), params->h());

    window->show(argc, argv);
    return Fl::run();
}
