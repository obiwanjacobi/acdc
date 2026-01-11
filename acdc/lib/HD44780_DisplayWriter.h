#pragma once
#include "./atl/ui/DisplayWriter.h"

/** An implementation for the DisplayWriter used in the UI framework for the HD44780 LCD display.
 *  The class derives from DisplayWriter and BaseT.
 *  \tparam BaseT is a TextWriter => HD44780_View => HD44780_Controller type that implements:
 *  `void Write(const char*)` (TextWriter)
 *  `uint8_t getCursorRow()` (HD44780_View)
 *  `uint8_t getCursorCol()` (HD44780_View)
 *  `void SetCursor(uint8_t, uint8_t)` (HD44780_View)
 *  `void setEnableCursor(bool)` (HD44780_Controller)
 *  `bool getEnableCursor()` (HD44780_Controller)
 *  `void setEnableBlink(bool)` (HD44780_Controller)
 *  `bool getEnableBlink()` (HD44780_Controller).
 */
template <class BaseT>
class HD44780_DisplayWriter : public DisplayWriter, public BaseT
{
public:
    /** Calls the `TextWriter::Write` method.
     *  \param text points to a zero-terminated string.
     */
    void Display(const char *text) override
    {
        BaseT::Write(text);
    }

    using BaseT::Write;

    /** Calls the `HD44780_View::SetCursor` method.
     *  Will also call `HD44780_View::getCursorRow` and `HD44780_View::getCursorCol`
     *  when `DisplayWriter::DontCare` values are specified.
     *  \param lineIndex indicates the display line.
     *  \param columnIndex indicates the display column (char position).
     */
    void GoTo(uint8_t lineIndex, uint8_t columnIndex) override
    {
        ResolveCurrentPos(lineIndex, columnIndex);
        BaseT::SetCursor(lineIndex, columnIndex);
    }

    /** Calls the `HD44780_View::SetCursor` and `HD44780_Controller::setEnableCursor` methods.
     *  May also call the `HD44780_Controller::setEnableBlink` method for edit mode.
     *  \param lineIndex indicates the display line.
     *  \param columnIndex indicates the display column (char position).
     *  \param edit indicates if a blinking cursor is displayed.
     */
    void EnableCursor(uint8_t lineIndex, uint8_t columnIndex, bool edit) override
    {
        if (lineIndex == CurrentPos && columnIndex == CurrentPos)
        {
            // This could be more efficient with SetDisplayControl
            // but that would also put another requirement on BaseT.
            BaseT::setEnableBlink(false);
            BaseT::setEnableCursor(false);
            return;
        }

        if (!BaseT::getEnableCursor())
        {
            BaseT::setEnableCursor(true);
        }

        if (BaseT::getEnableBlink() == edit)
        {
            BaseT::setEnableBlink(!edit);
        }

        GoTo(lineIndex, columnIndex);
    }

    using BaseT::SetCursor;

private:
    void ResolveCurrentPos(uint8_t &lineIndex, uint8_t &columnIndex)
    {
        if (lineIndex == CurrentPos)
            lineIndex = BaseT::getCursorRow();
        if (columnIndex == CurrentPos)
            columnIndex = BaseT::getCursorCol();
    }
};
