#include "fltk_parameters.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Spinner.H>
#include <FL/fl_draw.H>

#include <iostream>

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

/*
 * Überlegungen, was ich hier eigentlich haben will
 * Ich will angeben:
 *   - Parameter-Namen (zur Anzeige)
 *   - Parameter Namen im Struct
 *   - Minimalwert
 *   - Maximalwert
 *
 * Impl soll dann:
 *   - Neue Zeile hinzufügen mit Label und Spinner
 *   - Aktuellen Wert von Spinner auf Wert von Var setzen
 *   - Callback setzen, der Var auf Spinner-Wert setzt
 *   - Dabei min und max einhalten
 *   -
 */

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
        // TODO: delete other special member funcs
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
        grid->layout(5, 2, -1, 4);
        FormBuilder b{grid};

        b.add_row(add_new_parameter<int, &Params::adaptiveThreshConstant, 2, 4>("Thresh. const.:"));
        b.add_row(add_new_parameter<int, &Params::adaptiveThreshWinSizeMax, 2, 4>("Max. win. size:"));
        b.add_row(add_new_parameter<int, &Params::adaptiveThreshWinSizeMin, 2, 4>("Min. win. size:"));
        b.add_row(add_new_parameter<int, &Params::adaptiveThreshWinSizeStep, 2, 4>("Size step:"));

        this->end();
        this->fixed(title, hh);
        this->fixed(grid, 8 * hh);
        this->gap(5);
    }

    template <typename InputType, auto DataMember, InputType min, InputType max>
    constexpr std::pair<Fl_Box *, Fl_Spinner *> add_new_parameter(std::string_view name, std::string_view tooltip = "")
    {
        Fl_Box *label = new Fl_Box(0, 0, 0, 0, name.data());
        label->tooltip(tooltip.data());
        Fl_Spinner *spinner = new Fl_Spinner(0, 0, 0, 0);
        spinner->tooltip(tooltip.data());
        if constexpr (std::is_same_v<InputType, int>)
        {
            spinner->type(FL_INT_INPUT);
        }
        else
        {
            spinner->type(FL_FLOAT_INPUT);
            spinner->step(0.1);
        }
        spinner->value(m_params.*DataMember);
        add_callback<&ThresholdingParameters::set_value<DataMember, min, max>>(spinner, this);

        return {label, spinner};
    }

    template <auto DataMember, auto min, auto max> void set_value(Fl_Spinner *spinner)
    {
        auto new_value = spinner->value();
        std::cout << new_value << std::endl;
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
