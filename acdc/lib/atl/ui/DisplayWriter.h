#pragma once
#include <stdint.h>

/** The DisplayWriter is used to 'draw' Control instance onto a display.
 *  It detaches the details of how the display is driven from the Control UI framework.
 *  This class is abstract and cannot be instantiated and mainly serves as a template.
 *  See also HD44780_DisplayWriter for a full implementation.
 */
class DisplayWriter
{
public:
    /** This value can be passed to the `GoTo()` and `EnableCursor()` methods. */
    static const uint8_t CurrentPos = 0xFF;

    /** Writes the text to the current (cursor) position on the display.
     *  \param text points to a NULL terminated string.
     */
    virtual void Display(const char * /*text*/) {}

    /** Repositions the current cursor position to the specified lineIndex and columnIndex.
     *  \param lineIndex is a zero-based index for the line in the display.
     *  \param columnIndex is a zero-based index for the column in the display.
     */
    virtual void GoTo(uint8_t /*lineIndex*/, uint8_t /*columnIndex*/) {}

    /** Displays a cursor at the specified lineIndex and columnIndex.
     *  If edit is true, the edit-cursor is displayed.
     *  \param lineIndex is a zero-based index for the line in the display.
     *  \param columnIndex is a zero-based index for the column in the display.
     *  \param edit is an indication to display a 'edit' cursor
     */
    virtual void EnableCursor(uint8_t /*lineIndex*/, uint8_t /*columnIndex*/, bool /*edit*/) {}

    /** Removes the cursor from the display
     */
    void DisableCursor()
    {
        // secret speak to disable cursor and save one virtual in the v-table.
        EnableCursor(DisplayWriter::CurrentPos, DisplayWriter::CurrentPos, false);
    }

protected:
    DisplayWriter() {}
};
