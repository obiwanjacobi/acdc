#pragma once
#include <stdint.h>
#include "PanelControlContainer.h"

/** The VerticalScrollPanel implements a Control Panel where controls are lined up vertically.
 *  Note that controls need to Display content for their entire span (TextControl) or artifact
 *  during scrolling can be left behind of the 'line' previously at that location.
 *  \tparam MaxPageItems is the maximum number of Controls visible in the Panel.
 *  \tparam MaxScrollItems is the maximum number of Controls stored in the Panel.
 */
template <const uint8_t MaxPageItems, const uint8_t MaxScrollItems>
class VerticalScrollPanel : public PanelControlContainer<MaxScrollItems>
{
public:
    typedef PanelControlContainer<MaxScrollItems> BaseT;

    /** Constructs the instance with an option position.
     *  \param pos is the position relative to its siblings.
     */
    VerticalScrollPanel(uint8_t pos = 0)
        : BaseT(pos), _offset(0) {}

    /** Adds a Control to the panel. The Control's position is managed by
     *  the VerticalScrollPanel in order for it to function properly.
     *  \param control is the control to add. If NULL nothing happens.
     *  \return Returns false when the Panel is full or when control is null.
     */
    bool Add(Control *control)
    {
        if (BaseT::Add(control))
        {
            control->setPosition((uint8_t)BaseT::getCount() - 1);
            return true;
        }

        return false;
    }

    /** The Panel adjusts the current (selected) control on the `Up` and `Down` commands,
     *  if the BaseT (Panel) did not handle it.
     *  \param navCmd is the navigation command.
     *  \return Returns true when the command was handled, otherwise false.
     */
    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        bool handled = BaseT::OnNavigationCommand(navCmd);
        if (!handled)
        {
            switch (navCmd)
            {
            case NavigationCommands::Up:
                // LogTrace("Vsp:Nav-U");
                handled = SetPreviousInputControl();
                break;
            case NavigationCommands::Down:
                // LogTrace("Vsp:Nav-D");
                handled = SetNextInputControl();
                break;
            default:
                return false;
            }
        }

        return handled;
    }

    /** Displays all the visible controls in the Panel.
     *  This Display impl does not call the BaseT and all (visible) controls
     *  will be displayed at their positions.
     *  \param output is a pointer to the display writer that can be used to
     *      output and position text.
     */
    void Display(DisplayWriter *output) override
    {
        uint8_t pos = 0;
        for (uint8_t i = 0; i < MaxPageItems && i < BaseT::getCount(); i++)
        {
            Control *ctrl = BaseT::GetAt(i + _offset);
            if (ctrl->getIsVisible())
            {
                // the control position may be out of bounds of the page
                ctrl->setPosition(i);

                output->GoTo(pos, 0);
                ctrl->Display(output);

                // separate display position so hidden 'lines' will not leave gaps.
                pos++;
            }
        }
    }

    bool SetNextInputControl()
    {
        if (BaseT::SetNextInputControl())
        {
            InputControl *ctrl = BaseT::getCurrentControl();
            // cannot use ctrl->getPosition() because it is changed for display
            uint8_t pos = BaseT::IndexOf(ctrl);
            if (pos >= _offset + MaxPageItems)
                _offset++;

            return true;
        }
        return false;
    }

    bool SetPreviousInputControl()
    {
        if (BaseT::SetPreviousInputControl())
        {
            InputControl *ctrl = BaseT::getCurrentControl();
            // cannot use ctrl->getPosition() because it is changed for display
            uint8_t pos = BaseT::IndexOf(ctrl);
            if (pos < _offset)
                _offset--;

            return true;
        }
        return false;
    }

private:
    uint8_t _offset;
};