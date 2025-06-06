#include "fltk_parameters.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Spinner.H>
#include <FL/fl_draw.H>

template <auto callback_t, typename Parent, typename Widget> void add_callback(Widget *widget, Parent *self)
{
    widget->callback(
        [](Fl_Widget *widget, void *t) {
            // cast is guaranteed to be possible (see input types)
            ((static_cast<Parent *>(t))->*callback_t)(static_cast<Widget *>(widget));
        },
        static_cast<void *>(self));
}

static std::array<const char *, 22> dict_names{
    "DICT_4X4_50",         "DICT_4X4_100",        "DICT_4X4_250",       "DICT_4X4_1000",      "DICT_5X5_50",
    "DICT_5X5_100",        "DICT_5X5_250",        "DICT_5X5_1000",      "DICT_6X6_50",        "DICT_6X6_100",
    "DICT_6X6_250",        "DICT_6X6_1000",       "DICT_7X7_50",        "DICT_7X7_100",       "DICT_7X7_250",
    "DICT_7X7_1000",       "DICT_ARUCO_ORIGINAL", "DICT_APRILTAG_16h5", "DICT_APRILTAG_25h9", "DICT_APRILTAG_36h10",
    "DICT_APRILTAG_36h11", "DICT_ARUCO_MIP_36h12"};
// need unchanged enum; check only last value here...
static_assert(cv::aruco::PredefinedDictionaryType::DICT_ARUCO_MIP_36h12 == 21);

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

class ParametersImpl : public Fl_Flex
{
    using Params = cv::aruco::DetectorParameters;

  public:
    ParametersImpl(const ParametersImpl &) = delete;
    ParametersImpl(ParametersImpl &&) = delete;
    ParametersImpl &operator=(const ParametersImpl &) = delete;
    ParametersImpl &operator=(ParametersImpl &&) = delete;

    ParametersImpl(int x, int y, int w, int h,
                   std::function<void(cv::aruco::DetectorParameters, cv::aruco::PredefinedDictionaryType)> cb)
        : Fl_Flex(x, y, w, h, Fl_Flex::VERTICAL), m_callback(cb)
    {
        // --------------------- Dictionary Selection ------------------------------------------------

        Fl_Box *title = new Fl_Box(0, 0, 0, 0, "Dictionary");
        title->labelfont(FL_HELVETICA_BOLD);
        int ww = 0, hh = 0; // initialize to zero before calling fl_measure()
        title->measure_label(ww, hh);

        Fl_Choice *choice = new Fl_Choice(0, 0, 0, 0, "");
        for (auto name : dict_names)
        {
            choice->add(name);
        }
        choice->value(1);
        choice->callback(
            [](Fl_Widget *choice, void *data) {
                int selected = dynamic_cast<Fl_Choice *>(choice)->value();
                auto *self = static_cast<ParametersImpl *>(data);
                self->m_dict = cv::aruco::PredefinedDictionaryType(selected);
                self->m_callback(self->m_params, self->m_dict);
            },
            static_cast<void *>(this));
        this->fixed(title, hh);
        this->fixed(choice, 2 * hh);

        // --------------------- Thresholding  -------------------------------------------------------

        title = new Fl_Box(0, 0, 0, 0, "Thresholding");
        title->labelfont(FL_HELVETICA_BOLD);

        Fl_Grid *grid = new Fl_Grid(0, 0, 0, 0);
        grid->layout(4, 2, -1, 4);
        int col_weights[] = {50, 25};
        grid->col_weight(col_weights, sizeof(col_weights) / sizeof(col_weights[0]));
        FormBuilder b{grid};

        b.add_row(add_new_parameter<&Params::adaptiveThreshConstant, -999, 999>(
            "Thresh. const.:", "constant for adaptive thresholding before finding contours"));
        b.add_row(add_new_parameter<&Params::adaptiveThreshWinSizeMax, 4, 999, &Params::adaptiveThreshWinSizeMin>(
            "Max. win. size:", "maximum window size for adaptive thresholding before finding contours"));
        b.add_row(add_new_parameter<&Params::adaptiveThreshWinSizeMin, 3, 999, &Params::adaptiveThreshWinSizeMax>(
            "Min. win. size:", "minimum window size for adaptive thresholding before finding contours"));
        b.add_row(add_new_parameter<&Params::adaptiveThreshWinSizeStep, 1, 999>(
            "Size step:",
            "increments from adaptiveThreshWinSizeMin to adaptiveThreshWinSizeMax during the thresholding"));
        grid->end();
        this->fixed(title, hh);
        this->fixed(grid, 8 * hh);

        // --------------------- Filtering  ----------------------------------------------------------

        title = new Fl_Box(0, 0, 0, 0, "Filtering");
        title->labelfont(FL_HELVETICA_BOLD);
        grid = new Fl_Grid(0, 0, 0, 0);
        grid->layout(5, 2, -1, 4);
        grid->col_weight(col_weights, sizeof(col_weights) / sizeof(col_weights[0]));
        b = FormBuilder(grid);

        b.add_row(add_new_parameter<&Params::maxMarkerPerimeterRate, 0.0, 999.0, &Params::minMarkerPerimeterRate>(
            "Max. perimeter:", "determine maximum perimeter for marker contour to be detected"));
        b.add_row(add_new_parameter<&Params::minMarkerPerimeterRate, 0.0, 999.0, &Params::maxMarkerPerimeterRate>(
            "Min. perimeter:", "determine minimum perimeter for marker contour to be detected"));
        b.add_row(add_new_parameter<&Params::minCornerDistanceRate, 0.0, 999.0>(
            "Min. corner dist:", "minimum distance between corners for detected markers relative to its perimeter"));
        b.add_row(add_new_parameter<&Params::polygonalApproxAccuracyRate, 0.0, 999.0>(
            "Poly. approx. rate:",
            "minimum accuracy during the polygonal approximation process to determine which contours are squares"));
        b.add_row(add_new_parameter<&Params::minMarkerDistanceRate, 0.0, 999.0>(
            "Min. marker dist. rate:", "minimum mean distance beetween two marker corners to be considered imilar, so "
                                       "that the smaller one is removed"));
        grid->end();
        this->end();
        this->fixed(title, hh);
        this->fixed(grid, 10 * hh);
        this->gap(5);
    }

    template <auto DataMember, auto min, auto max, auto OtherMember = DataMember>
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
        // min and max slightly larger, such that our own custom logic handles over- and underflow
        spinner->minimum(min - 1);
        spinner->maximum(max + 1);
        spinner->value(m_params.*DataMember);
        add_callback<&ParametersImpl::set_value<DataMember, min, max, OtherMember>>(spinner, this);

        return {label, spinner};
    }

    template <auto DataMember, auto min, auto max, auto OtherMember> void set_value(Fl_Spinner *spinner)
    {
        auto new_value = spinner->value();
        if (new_value < min || new_value > max)
        {
            spinner->value(m_params.*DataMember);
        }
        else
        {
            if (m_params.*DataMember > m_params.*OtherMember)
            {
                // max value of min/max pair, needs to stay larger
                if (m_params.*OtherMember >= new_value)
                {
                    spinner->value(m_params.*DataMember);
                }
                else
                {
                    m_params.*DataMember =
                        static_cast<std::remove_cvref_t<decltype(m_params.*DataMember)>>(spinner->value());
                }
            }
            else if (m_params.*DataMember < m_params.*OtherMember)
            {
                // min value of min/max pair, needs to stay smaller
                if (m_params.*OtherMember <= new_value)
                {
                    spinner->value(m_params.*DataMember);
                }
                else
                {
                    m_params.*DataMember =
                        static_cast<std::remove_cvref_t<decltype(m_params.*DataMember)>>(spinner->value());
                }
            }
            else
            {
                // not part of min max pair; can have any value within own minimum and maximum value
                m_params.*DataMember =
                    static_cast<std::remove_cvref_t<decltype(m_params.*DataMember)>>(spinner->value());
            }
        }

        // callback to re-compute with new params
        m_callback(m_params, m_dict);
    }

  private:
    cv::aruco::DetectorParameters m_params;
    cv::aruco::PredefinedDictionaryType m_dict;
    std::function<void(cv::aruco::DetectorParameters, cv::aruco::PredefinedDictionaryType)> m_callback;
};

Parameters::Parameters(int x, int y, int w, int h,
                       std::function<void(cv::aruco::DetectorParameters, cv::aruco::PredefinedDictionaryType)> cb)
    : Fl_Flex(x, y, w, h, Fl_Flex::VERTICAL)
{
    new ParametersImpl(0, 0, w, h, cb);
    this->end();
}
