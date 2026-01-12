#pragma once
#include "../Collection.h"
#include "../FixedArray.h"

/** The ControlContainer represents a collection of `ControlBase`s.
 *  It uses Collection and FixedArray to implement the collection.
 *  \tparam MaxItems is the maximum number of controls in the container.
 */
template <const uint8_t MaxItems>
class ControlContainer : public Collection<FixedArray<Control *, MaxItems>>
{
    typedef Collection<FixedArray<Control *, MaxItems>> BaseT;

public:
    /** Retrieves the Control that is next to the currentCtrl.
     *  \param currentCtrl is a pointer to the Control to use as reference.
     *  \param type is the type of Control to look for. Uses ControlCast.
     *  \return Returns NULL if no suitable Control (type) could be found.
     */
    Control *getNext(Control *currentCtrl, ControlTypes type = ControlTypes::Control) const
    {
        int16_t index = 0;
        if (currentCtrl != nullptr)
        {
            index = BaseT::IndexOf(currentCtrl);
            if (index == -1)
                return nullptr;
            index++;
        }

        while (index >= 0 && index < BaseT::getCount())
        {
            Control *ctrl = ControlCast(BaseT::GetAt(index), type);

            if (ctrl != nullptr)
                return ctrl;

            index++;
        }

        return nullptr;
    }

    /** Retrieves the Control that is before the currentCtrl.
     *  \param currentCtrl is a pointer to the Control to use as reference.
     *  \param type is the type of Control to look for. Uses ControlCast.
     *  \return Returns NULL if no suitable Control (type) could be found.
     */
    Control *getPrevious(Control *currentCtrl, ControlTypes type = ControlTypes::Control) const
    {
        int16_t index = 0;
        if (currentCtrl != nullptr)
        {
            index = BaseT::IndexOf(currentCtrl);
            if (index == -1)
                return nullptr;
            index--;
        }

        while (index >= 0 && index < BaseT::getCount())
        {
            Control *ctrl = ControlCast(BaseT::GetAt(index), type);

            if (ctrl != nullptr)
                return ctrl;

            index--;
        }

        return nullptr;
    }

    /** Overrides Collection::Add to disallow NULL pointers in the collection.
     *  \param control is the control to add. If NULL nothing happens.
     *  \return Returns true when successful.
     */
    bool Add(Control *control)
    {
        if (control == nullptr)
            return false;

        return BaseT::Add(control);
    }
};
