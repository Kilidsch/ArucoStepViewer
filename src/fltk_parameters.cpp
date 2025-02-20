#include "fltk_parameters.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Spinner.H>
#include <FL/fl_draw.H>

template <auto callback_t, typename Parent, typename Widget> void add_callback(Widget *widget, Parent *self)
{
    widget->callback(
        [](Fl_Widget *widget, void *t) {
            // dynamic casts not possible with void pointer
            // but this is guaranteed to be possible (see input types)
            // so the C-style may hopefully be forgiven
            (((Parent *)t)->*callback_t)((Widget *)widget);
        },
        (void *)self);
}

class FormBuilder
{
    int m_row = 0;
    Fl_Grid *m_grid;

  public:
    // bookkeeping with row builds on this being the only one
    FormBuilder(const FormBuilder &) = delete;
    FormBuilder(FormBuilder &&) = default;
    FormBuilder &operator=(const FormBuilder &) = delete;
    FormBuilder &operator=(FormBuilder &&) = default;
    FormBuilder(Fl_Grid *grid) : m_grid(grid)
    {
    }

    void add_row(std::pair<Fl_Box *, Fl_Spinner *> widgets)
    {
        auto [label, spinner] = widgets;
        m_grid->widget(label, m_row, 0);
        m_grid->widget(spinner, m_row, 1);
        m_row++;
    }
};

class ThresholdingParameters : Fl_Flex
{
    using Params = cv::aruco::DetectorParameters;

  public:
    ThresholdingParameters(const ThresholdingParameters &) = delete;
    ThresholdingParameters(ThresholdingParameters &&) = delete;
    ThresholdingParameters &operator=(const ThresholdingParameters &) = delete;
    ThresholdingParameters &operator=(ThresholdingParameters &&) = delete;

    ThresholdingParameters(int x, int y, int w, int h, cv::aruco::DetectorParameters &params)
        : Fl_Flex(x, y, w, h, Fl_Flex::VERTICAL), m_params(params)
    {
        Fl_Box *title = new Fl_Box(0, 0, 0, 0, "Thresholding");
        title->labelfont(FL_HELVETICA_BOLD);
        int ww = 0, hh = 0; // initialize to zero before calling fl_measure()
        title->measure_label(ww, hh);

        Fl_Grid *grid = new Fl_Grid(0, 0, 0, 0);
        grid->layout(4, 2, -1, 4);
        FormBuilder b{grid};

        // TODO: special error handling to ensure minWinSize < maxWinSize
        b.add_row(add_new_parameter<&Params::adaptiveThreshConstant, -999, 999>(
            "Thresh. const.:", "constant for adaptive thresholding before finding contours"));
        b.add_row(add_new_parameter<&Params::adaptiveThreshWinSizeMax, 4, 999>(
            "Max. win. size:", "maximum window size for adaptive thresholding before finding contours"));
        b.add_row(add_new_parameter<&Params::adaptiveThreshWinSizeMin, 3, 999>(
            "Min. win. size:", "minimum window size for adaptive thresholding before finding contours"));
        b.add_row(add_new_parameter<&Params::adaptiveThreshWinSizeStep, 1, 999>(
            "Size step:",
            "increments from adaptiveThreshWinSizeMin to adaptiveThreshWinSizeMax during the thresholding"));
        grid->end();
        this->fixed(title, hh);
        this->fixed(grid, 8 * hh);

        title = new Fl_Box(0, 0, 0, 0, "Filtering");
        title->labelfont(FL_HELVETICA_BOLD);
        grid = new Fl_Grid(0, 0, 0, 0);
        grid->layout(5, 2, -1, 4);
        b = FormBuilder(grid);

        // TODO: ensure minMarkerPeri < maxMarkerPeri
        b.add_row(add_new_parameter<&Params::maxMarkerPerimeterRate, 0.0, 999.0>(
            "Max. perimeter:", "determine maximum perimeter for marker contour to be detected"));
        b.add_row(add_new_parameter<&Params::minMarkerPerimeterRate, 0.0, 999.0>(
            "Min. perimeter:", "determine minimum perimeter for marker contour to be detected"));
        b.add_row(add_new_parameter<&Params::minCornerDistanceRate, 0.0, 999.0>(
            "Min. corner dist:", "minimum distance between corners for detected markers relative to its perimeter"));
        b.add_row(add_new_parameter<&Params::polygonalApproxAccuracyRate, 0.0, 999.0>(
            "Poly. approx. rate:",
            "minimum accuracy during the polygonal approximation process to determine which contours are squares"));
        b.add_row(add_new_parameter<&Params::minMarkerDistanceRate, 0.0, 999.0>(
            "Min. marker dist. rate:", "minimum mean distance beetween two marker corners to be considered imilar, so "
                                       "that the smaller one is removed"));

        this->end();
        this->fixed(title, hh);
        this->fixed(grid, 10 * hh);
        this->gap(5);
    }

    template <auto DataMember, auto min, auto max>
        requires std::is_same_v<decltype(min), decltype(max)>
    constexpr std::pair<Fl_Box *, Fl_Spinner *> add_new_parameter(std::string_view name, std::string_view tooltip = "")
    {
        Fl_Box *label = new Fl_Box(0, 0, 0, 0, name.data());
        label->tooltip(tooltip.data());
        Fl_Spinner *spinner = new Fl_Spinner(0, 0, 0, 0);
        spinner->tooltip(tooltip.data());
        if constexpr (std::is_same_v<decltype(min), int>)
        {
            spinner->type(FL_INT_INPUT);
        }
        else
        {
            spinner->type(FL_FLOAT_INPUT);
            spinner->step(0.01);
        }
        spinner->value(m_params.*DataMember);
        add_callback<&ThresholdingParameters::set_value<DataMember, min, max>>(spinner, this);

        return {label, spinner};
    }

    template <auto DataMember, auto min, auto max> void set_value(Fl_Spinner *spinner)
    {
        auto new_value = spinner->value();
        if (new_value < min || new_value > max)
        {
            spinner->value(m_params.*DataMember);
        }
        else
        {
            // Cast is just to we do not get nagged at by clangd, because of the implicit cast to int if the DataMember
            // is int
            m_params.*DataMember = static_cast<std::remove_cvref_t<decltype(m_params.*DataMember)>>(spinner->value());
        }
    }

  private:
    cv::aruco::DetectorParameters &m_params;
};

Parameters::Parameters(int x, int y, int w, int h) : Fl_Flex(x, y, w, h, Fl_Flex::VERTICAL)
{
    new ThresholdingParameters(0, 0, w, h, m_params);
    this->end();
}
