#pragma once
#include <stdint.h>

#include "Block.h"
#include "Commands.h"

extern Serial serial;

BlockControllerT_0 blockController0;
BlockControllerT_1 blockController1;
BlockControllerT_2 blockController2;
BlockControllerT_3 blockController3;

class CommandHandler
{
public:
    bool Open()
    {
        return blockController0.Open() &&
               blockController1.Open() &&
               blockController2.Open() &&
               blockController3.Open();
    }

    bool TryCreateBlockOccupationEvent(BlockOccupationEvent **outEvent)
    {
        if (blockController0.TryReadOccupied() ||
            blockController1.TryReadOccupied() ||
            blockController2.TryReadOccupied() ||
            blockController3.TryReadOccupied())
        {
            BitArray<uint8_t> flags;
            flags.Set(0, blockController0.getOccupied());
            flags.Set(1, blockController1.getOccupied());
            flags.Set(2, blockController2.getOccupied());
            flags.Set(3, blockController3.getOccupied());

            *outEvent = BlockOccupationEvent::Create(1, flags);
            return true;
        }

        return false;
    }

protected:
    void OnCommand(GlobalResetCommand *command)
    {
    }

    void OnCommand(BlockPowerCommand *command)
    {
        switch (command->BlockId)
        {
        case 1:
            blockController0.setPower(command->PowerOn);
            break;
        case 2:
            blockController1.setPower(command->PowerOn);
            break;
        case 3:
            blockController2.setPower(command->PowerOn);
            break;
        case 4:
            blockController3.setPower(command->PowerOn);
            break;
        default:
            break;
        }
    }
    void OnCommand(BlockSpeedCommand *command)
    {
        switch (command->BlockId)
        {
        case 1:
            blockController0.setSpeed(command->Speed);
            // serial.Transmit.Write("B1S");
            break;
        case 2:
            blockController1.setSpeed(command->Speed);
            // serial.Transmit.Write("B2S");
            break;
        case 3:
            blockController2.setSpeed(command->Speed);
            // serial.Transmit.Write("B3S");
            break;
        case 4:
            blockController3.setSpeed(command->Speed);
            // serial.Transmit.Write("B4S");
            break;
        default:
            break;
        }

        // serial.Transmit.WriteLine(command->Speed);
    }
};
