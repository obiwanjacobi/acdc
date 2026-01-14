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

    /** Returns the current scroll offset.
     *  \return Returns the scroll offset.
     */
    uint8_t getScrollPosition() const
    {
        return _offset;
    }

    /** Scrolls the display to the specified position.
     *  \param scrollPos the character position to scroll to.
     *  \return Returns false if the scrollPos is invalid.
     */
    bool setScrollPosition(uint8_t scrollPos)
    {
        if (scrollPos < 0 || scrollPos > BaseT::getTotalColumns())
            return false;

        int8_t diff = _offset - scrollPos;
        BaseT::Direction dir = BaseT::Direction::Left;
        if (diff < 0)
            dir = Base::Direction::Right;

        for (int8_t i = 0; i < Math::Abs(diff); i++)
            BaseT::WriteCursorShift(true, dir);

        _offset = scrollPos;
        return true;
    }

    /** Scrolls the text one position to the right.
     *  Does not allow to scroll beyond the end of the display.
     *  \return Returns false when the scroll could not performed.
     */
    bool ScrollRight()
    {
        if (_offset > 0)
        {
            _offset--;
            BaseT::WriteCursorShift(true, BaseT::Direction::Right);
            return true;
        }

        return false;
    }

    /** Scrolls the text one position to the left.
     *  Does not allow to scroll beyond the start of the display.
     *  \return Returns false when the scroll could not performed.
     */
    bool ScrollLeft()
    {
        if (_offset > BaseT::getTotalColumns())
        {
            _offset++;
            BaseT::WriteCursorShift(true, BaseT::Direction::Left);
            return true;
        }

        return false;
    }

private:
    uint8_t _offset;
};