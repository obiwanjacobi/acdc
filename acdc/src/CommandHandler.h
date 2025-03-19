#pragma once
#include <stdint.h>

struct CommandHandlerParams
{
    // for writing back responses on executed commands
    SerialWriter *writer;
    //MotorController *motorController;
};

class CommandHandler
{
public:
    CommandHandler(CommandHandlerParams &params)
    {
        _writer = params.writer;
        //_motorController = params.motorController;
    }

    void OnPower(bool on, uint8_t track)
    {
        if (_writer)
            _writer->WriteLine("OnPower");

        // do something
    }

private:
    SerialWriter *_writer;
    //MotorController *_motorController;
};
