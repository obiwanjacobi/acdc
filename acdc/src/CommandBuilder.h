#pragma once
#include <stdint.h>

#include "Commands.h"

// command: the basic command ids
// data: a buffer with command parameter data
// typedCommand: receives a pointer to the typed command
// return: true when successfull - typedCommand is filled (non-null).
bool TryBuildTypedCommand(Command &command, Slice<uint8_t> &data, Command **typedCommand)
{
    // global commands
    if (GlobalCommand::TryCreate(command, data, (GlobalCommand **)typedCommand))
        return true;

    // node commands
    if (NodeCommand::TryCreate(command, data, (NodeCommand **)typedCommand))
        return true;

    // device commands

    *typedCommand = nullptr;
    return false;
};
