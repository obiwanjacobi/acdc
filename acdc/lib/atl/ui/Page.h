#pragma once
#include <stdint.h>
#include "VerticalPanel.h"

/** The Page class manages the Lines that are displayed together as one (page).
 *  Lines run horizontally and are organized in a VerticalPanel.
 *  \tparam MaxLines is the maximum number of lines on the Page.
 *  \tparam BaseT a `VerticalPanel` compatible base class.
 *      `Add(Panel*)`
 *      `Display(DisplayWriter*)`
 *      `setCurrentControl(InputControl*)`
 *      `SetNextInputControl()`
 *      `SetPreviousInputControl()`
 */
template <const uint8_t MaxLines, class BaseT = VerticalPanel<MaxLines>>
class Page : public BaseT
{
public:
    typedef Panel ItemT;

    /** Constructs the instance.
     *  Lines have to be added using the Add method.
     */
    Page() {}

    /** Constructs the instance.
     *  Lines can be added using the Add method.
     *  \param line1 is the first line.
     */
    Page(ItemT *line1)
    {
        static_assert(MaxLines >= 1);
        BaseT::Add(line1);
    }

    /** Constructs the instance.
     *  Lines can be added using the Add method.
     *  \param line1 is the first line.
     *  \param line2 is the second line.
     */
    Page(ItemT *line1, ItemT *line2)
    {
        static_assert(MaxLines >= 2);
        BaseT::Add(line1);
        BaseT::Add(line2);
    }

    /** Constructs the instance.
     *  Lines can be added using the Add method.
     *  \param line1 is the first line.
     *  \param line2 is the second line.
     *  \param line3 is the third line.
     *  \param line4 is the fourth line.
     */
    Page(ItemT *line1, ItemT *line2, ItemT *line3, ItemT *line4)
    {
        static_assert(MaxLines >= 4);
        BaseT::Add(line1);
        BaseT::Add(line2);
        BaseT::Add(line3);
        BaseT::Add(line4);
    }

    /** Calls `Display` on all its lines.
     *  This method also calls `DisplayCursor` for drawing the cursor.
     *  \param output is used to output text and position the cursor.
     */
    void Display(DisplayWriter *output) override
    {
        BaseT::Display(output);
        DisplayCursor(output);
    }

    /** Implements displaying the cursor.
     *  Calls `DisplayCursor()` on the current control on the current line
     *  or turns the cursor of if no current control is available.
     *  \param output is used to set the cursor position.
     */
    void DisplayCursor(DisplayWriter *output) override
    {
        InputControl *ctrl = getCurrentInputControl();

        if (ctrl != nullptr && ctrl->getIsActive())
        {
            ItemT *line = getCurrentLine();
            output->EnableCursor(line->getPosition(), ctrl->getPosition(), ctrl->getIsSelected());

            ctrl->DisplayCursor(output);
        }
        else
        {
            // cursor off
            output->DisableCursor();
        }
    }

    /** Routes navigation commands to the active line.
     *  If no Control in the `stateSelected` is present the `Up` and `Down` commands change
     *  the current line.
     *  \param navCmd is the navigation command.
     */
    bool OnNavigationCommand(NavigationCommands navCmd) override
    {
        // LogTrace("Pg:Nav");
        bool handled = false;

        switch (navCmd)
        {
        case NavigationCommands::Up:
            // LogTrace("Pg:Nav:Ln-Up");
            handled = TrySelectPreviousLine();
            break;
        case NavigationCommands::Down:
            // LogTrace("Pg:Nav:Ln-Dwn");
            handled = TrySelectNextLine();
            break;
        case NavigationCommands::Exit:
            // LogTrace("Pg:Nav:CurCtrl-Ext");
            handled = TryUnselectCurrentControl();
            break;
        default:
            break;
        }

        if (!handled)
            // Skip BaseT (VerticalPanel) because we reimplemented line navigation (up/down) here.
            handled = this->BaseT::BaseT::OnNavigationCommand(navCmd);

        return handled;
    }

    /** Overridden to implement the typePage type flag.
     *  \param type indicates the requested type.
     *  \return Returns true if the type flag matches the class hierarchy.
     */
    bool IsOfType(ControlTypes type) const override
    {
        return HasFlag(type, ControlTypes::Page) || BaseT::IsOfType(type);
    }

    /** Un-selects the current focused control (if any).
     *  \return Returns true if the current control was unselected.
     */
    bool TryUnselectCurrentControl()
    {
        InputControl *currentCtrl = getCurrentInputControl();

        if (currentCtrl != nullptr &&
            currentCtrl->getIsFocused())
        {
            BaseT::setCurrentControl(nullptr);
            return true;
        }

        return false;
    }

    /** Selects the next line -or the first if no current line is set- if no Control is currently selected.
     *  \return Returns true if successful.
     */
    bool TrySelectNextLine()
    {
        // LogTrace("Pg:SelNxtLn");
        InputControl *currentCtrl = getCurrentInputControl();

        if (currentCtrl == nullptr ||
            (currentCtrl != nullptr && !currentCtrl->getIsSelected()))
        {
            if (BaseT::SetNextInputControl())
            {
                TryFocusFirstControl();
                // LogTrace("Pg:SelNxtLn-ok");
                return true;
            }
        }

        return false;
    }

    /** Selects the previous line -or the last if no current line is set- if no Control is currently selected.
     *  \return Returns true if successful.
     */
    bool TrySelectPreviousLine()
    {
        InputControl *currentCtrl = getCurrentInputControl();

        if (currentCtrl == nullptr ||
            (currentCtrl != nullptr && !currentCtrl->getIsSelected()))
        {
            if (BaseT::SetPreviousInputControl())
            {
                TryFocusFirstControl();
                return true;
            }
        }

        return false;
    }

    /** Retrieves the current line.
     *  \return Returns NULL if no current line is available.
     */
    inline ItemT *getCurrentLine() const
    {
        return (ItemT *)BaseT::getCurrentControl();
    }

    /** Retrieves the Control on the current line that is focused or selected.
     *  \return Returns NULL if no current line or current control is available.
     */
    InputControl *getCurrentInputControl() const
    {
        ItemT *currentLine = getCurrentLine();

        if (currentLine != nullptr)
            return currentLine->getCurrentControl();

        // LogTrace("Pg:CurCtrl=0");
        return nullptr;
    }

protected:
    /** Focuses the first control of the current line if no current control is set.
     *  \return Returns true if the control state was changed.
     */
    inline bool TryFocusFirstControl()
    {
        ItemT *line = getCurrentLine();

        if (line != nullptr &&
            line->getCurrentControl() == nullptr)
        {
            // LogTrace("Pg:Fcs1");
            // Panel has no way to access the control collection
            // This could fail if the lines are not PanelControlContainer
            return line->OnNavigationCommand(NavigationCommands::Right);
        }

        return false;
    }
};