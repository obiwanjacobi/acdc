#pragma once

/** The controller class implements the logic of how to talk to the TB6612FNG chip
 *  but leaving the details of how the chip is actually connected to the driver class.
 *  See also the TB6612FNG_Driver template class.
 *  \tparam BaseT is used as a base class and implements:
 *  `void Send(bool in1, bool in2, uint8_t pwm)` (TB6612FNG_Driver)
 */
template <class BaseT>
class TB6612FNG_Controller : public BaseT
{
public:
    /** Sends a stop command.
     */
    void
    Stop()
    {
        BaseT::Send(false, false, 0);
    }

    /** Sends a break command.
     */
    void Break()
    {
        BaseT::Send(true, true, 0);
    }

    /** Sends a 'turn the motor clockwise' command.
     *  \param speed is value between 0 (slow) and 255 (fast).
     */
    void Clockwise(uint8_t speed)
    {
        BaseT::Send(true, false, speed);
    }

    /** Sends a 'turn the motor counter-clockwise' command.
     *  \param speed is value between 0 (slow) and 255 (fast).
     */
    void CounterClockwise(uint8_t speed)
    {
        BaseT::Send(false, true, speed);
    }
};