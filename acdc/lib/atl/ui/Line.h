#pragma once
#include <stdint.h>
#include "HorizontalPanel.h"

/** The Line class represent a single line on a display that may contain multiple Controls.
 *  The Line class derives from the HorizontalPanel class.
 *  \tparam MaxItems is the maximum number of Controls on the Line.
 */
template <const uint8_t MaxItems>
class Line : public HorizontalPanel<MaxItems>
{
public:
    typedef HorizontalPanel<MaxItems> BaseT;

    /** Constructs the instance.
     *  Controls have to be added with the Add method.
     */
    Line(uint8_t pos = 0)
        : BaseT(pos)
    {
    }

    /** Constructs the instance.
     *  Controls can be added with the Add method.
     *  \param control1 is the first control.
     */
    Line(Control *control1, uint8_t pos = 0)
        : BaseT(pos)
    {
        static_assert(MaxItems >= 1);
        BaseT::Add(control1);
    }

    /** Constructs the instance.
     *  Controls can be added with the Add method.
     *  \param control1 is the first control.
     *  \param control2 is the second control.
     */
    Line(Control *control1, Control *control2, uint8_t pos = 0)
        : BaseT(pos)
    {
        static_assert(MaxItems >= 2);
        BaseT::Add(control1);
        BaseT::Add(control2);
    }

    /** Constructs the instance.
     *  Controls can be added with the Add method.
     *  \param control1 is the first control.
     *  \param control2 is the second control.
     *  \param control3 is the third control.
     */
    Line(Control *control1, Control *control2, Control *control3, uint8_t pos = 0)
        : BaseT(pos)
    {
        static_assert(MaxItems >= 3);
        BaseT::Add(control1);
        BaseT::Add(control2);
        BaseT::Add(control3);
    }

    /** Constructs the instance.
     *  Controls can be added with the Add method.
     *  \param control1 is the first control.
     *  \param control2 is the second control.
     *  \param control3 is the third control.
     *  \param control4 is the fourth control.
     */
    Line(Control *control1, Control *control2, Control *control3, Control *control4, uint8_t pos = 0)
        : BaseT(pos)
    {
        static_assert(MaxItems >= 4);
        BaseT::Add(control1);
        BaseT::Add(control2);
        BaseT::Add(control3);
        BaseT::Add(control4);
    }

    /** Constructs the instance.
     *  Controls can be added with the Add method.
     *  \param control1 is the first control.
     *  \param control2 is the second control.
     *  \param control3 is the third control.
     *  \param control4 is the fourth control.
     *  \param control5 is the fifth control.
     */
    Line(Control *control1, Control *control2, Control *control3, Control *control4, Control *control5, uint8_t pos = 0)
        : BaseT(pos)
    {
        static_assert(MaxItems >= 5);
        BaseT::Add(control1);
        BaseT::Add(control2);
        BaseT::Add(control3);
        BaseT::Add(control4);
        BaseT::Add(control5);
    }
};