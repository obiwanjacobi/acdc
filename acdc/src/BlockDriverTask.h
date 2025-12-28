#pragma once
#include <stdint.h>
#include "../lib/atl/Collection.h"
#include "../lib/atl/FixedArray.h"
#include "../lib/atl/Task.h"
#include "Block.h"
// #include "CurrentBlockController.h"
#include "OptoBlockController.h"
#include "hardware.h"
#include "Serial.h"

extern Serial serial;

// Controls the StopBlock based on the state of the PrioBlock
template <class SchedulerT, class StopBlockT, class PrioBlockT>
class BlockDriverTask
{
public:
    BlockDriverTask()
    {
        _state = State::Running;
    }

    enum class State : uint8_t
    {
        Conflict, // detected both block occupied
        Stopped,  // stopped the stopBlock
        Running,  // stopBlock is running
    };

    bool AdditionalProcessing(StopBlockT &stopBlock, PrioBlockT &prioBlock, const char *stopMessage)
    {
        if (_state == State::Conflict)
            return Run(stopBlock, prioBlock, stopMessage);

        return true;
    }

    Task_BeginParams(Run, StopBlockT &stopBlock, PrioBlockT &prioBlock, const char *stopMessage)
    {
        bool stopOccupied = stopBlock.getOccupied();
        bool prioOccupied = prioBlock.getOccupied();

        if (_state == State::Conflict)
        {
            // could be the train transitioning from stopBlock to prioBlok, so wait a bit
            Task_WaitUntil(SchedulerT::Wait((uint16_t)stopMessage, SchedulerT::TimeForMilliseconds(200)));
        }

        if (_state == State::Stopped)
        {
            // previously stopped
            if (!prioOccupied)
            {
                // we are stopped but the next block has cleared
                // we can start again
                stopBlock.setSpeed(_speed);
                _state = State::Running;
            }
        }
        else if (_state == State::Conflict)
        {
            // are they still both occupied?
            if (stopOccupied && prioOccupied)
            {
                stopBlock.setSpeed(0);
                _state = State::Stopped;

                // serial.Transmit.WriteLine(stopMessage);
            }
        }
        else // running
        {
            if (stopOccupied && prioOccupied)
            {
                _state = State::Conflict;
            }
        }
    }
    Task_End;

    uint16_t getId() const
    {
        return (uint16_t)this;
    }

    void setSpeed(uint8_t speed)
    {
        _speed = speed;
    }

private:
    uint16_t _task;
    State _state;
    uint8_t _speed;
    uint32_t _time;
};

template <class SchedulerT>
class BlockControllerTask
{
public:
    BlockControllerTask()
    {
        _detected = false;
        _speed = 100;
    }

    Task_BeginParams(Run, BlockControllerT_0 &block0, BlockControllerT_1 &block1, BlockControllerT_2 &block2, BlockControllerT_3 &block3)
    {
        if (!_detected)
        {
            block0.setSpeed(_speed);
            block1.setSpeed(_speed);
            block2.setSpeed(_speed);
            block3.setSpeed(_speed);

            _blockDriver0.setSpeed(_speed);
            _blockDriver1.setSpeed(_speed);
            _blockDriver2.setSpeed(_speed);
            _blockDriver3.setSpeed(_speed);

            _detected = true;
        }

        while (true)
        {
            if (block0.TryReadOccupied() ||
                block1.TryReadOccupied() ||
                block2.TryReadOccupied() ||
                block3.TryReadOccupied())
            {
                // serial.Transmit.Write(block0.getOccupied());
                // serial.Transmit.Write(block1.getOccupied());
                // serial.Transmit.Write(block2.getOccupied());
                // serial.Transmit.WriteLine(block3.getOccupied());

                _blockDriver0.Run(block0, block1, "Stop 0");
                _blockDriver1.Run(block1, block2, "Stop 1");
                _blockDriver2.Run(block2, block3, "Stop 2");
                _blockDriver3.Run(block3, block0, "Stop 3");
            }
            else
            {
                _blockDriver0.AdditionalProcessing(block0, block1, "Stop 0");
                _blockDriver1.AdditionalProcessing(block1, block2, "Stop 1");
                _blockDriver2.AdditionalProcessing(block2, block3, "Stop 2");
                _blockDriver3.AdditionalProcessing(block3, block0, "Stop 3");
            }

            Task_Yield();
        }
    }
    Task_End;

    void setSpeed(uint8_t speed)
    {
        _speed = speed;
    }

    uint16_t getId() const
    {
        return (uint16_t)this;
    }

private:
    bool _detected;
    uint8_t _speed;
    uint8_t _task;

    BlockDriverTask<SchedulerT, BlockControllerT_0, BlockControllerT_1> _blockDriver0;
    BlockDriverTask<SchedulerT, BlockControllerT_1, BlockControllerT_2> _blockDriver1;
    BlockDriverTask<SchedulerT, BlockControllerT_2, BlockControllerT_3> _blockDriver2;
    BlockDriverTask<SchedulerT, BlockControllerT_3, BlockControllerT_0> _blockDriver3;
};
