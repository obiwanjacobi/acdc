#pragma once
#include <stdint.h>
#include "ControlContainer.h"
#include "Panel.h"

/** The PanelControlContainer abstract class that manages a collection of Controls.
 *  \tparam MaxItems is the maximum number of Controls in the container.
 */
template <const uint8_t MaxItems>
class PanelControlContainer : public Panel,
                              public ControlContainer<MaxItems>
{
public:
    typedef ControlContainer<MaxItems> BaseT;

    /** Assigns the first InputControl in the container to be the current control (Panel).
     *  \return Returns true if successful.
     */
    bool SetFirstInputControl()
    {
        InputControl *ctrl = (InputControl *)BaseT::getNext(nullptr, ControlTypes::InputControl);
        if (ctrl == nullptr)
            return false;

        Panel::setCurrentControl(ctrl);
        return true;
    }

    /** Assigns the next InputControl (relative to the current current-control) in the container to be the current control (Panel).
     *  \return Returns true if successful.
     */
    bool SetNextInputControl()
    {
        InputControl *ctrl = (InputControl *)BaseT::getNext(Panel::getCurrentControl(), ControlTypes::InputControl);
        if (ctrl == nullptr)
            return false;

        Panel::setCurrentControl(ctrl);
        return true;
    }

    /** Assigns the previous InputControl (relative to the current current-control) in the container to be the current control (Panel).
     *  \return Returns true if successful.
     */
    bool SetPreviousInputControl()
    {
        InputControl *ctrl = (InputControl *)BaseT::getPrevious(Panel::getCurrentControl(), ControlTypes::InputControl);
        if (ctrl == nullptr)
            return false;

        Panel::setCurrentControl(ctrl);
        return true;
    }

protected:
    /** For derived classes.
     *  \param pos is the optional control position.
     */
    PanelControlContainer(uint8_t pos = 0)
        : Panel(pos, nullptr)
    {
    }
};