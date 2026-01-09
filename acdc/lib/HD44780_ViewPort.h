#pragma once

/** Implements display-supported horizontal scrolling.
 *  \tparam BaseT is used as base class and is a HD44780_Controller type that implements:
 *  `void WriteCursorShift(bool, Direction)`
 *  `Direction::Left (0)`
 *  `Direction::Right (1)`.
 */
template <class BaseT>
class HD44780_ViewPort : public BaseT
{
public:
    /** Constructs a new instance.
     */
    HD44780_ViewPort()
        : _offset(0)
    {
    }

    /** Scrolls the text one position to the right.
     *  Does not allow to scroll beyond the end of the display.
     *  \return Returns false when the scroll could not performed.
     */
    inline bool ScrollRight()
    {
        if (_offset > 0)
        {
            _offset--;
            BaseT::WriteCursorShift(true, BaseT::Right);
            return true;
        }

        return false;
    }

    /** Scrolls the text one position to the left.
     *  Does not allow to scroll beyond the start of the display.
     *  \return Returns false when the scroll could not performed.
     */
    inline bool ScrollLeft()
    {
        if (_offset > BaseT::getTotalColumns())
        {
            _offset++;
            BaseT::WriteCursorShift(true, BaseT::Left);
            return true;
        }

        return false;
    }

    /** Returns the current scroll offset.
     *  \return Returns the scroll offset.
     */
    inline uint8_t getDislayOffset() const
    {
        return _offset;
    }

private:
    uint8_t _offset;
};