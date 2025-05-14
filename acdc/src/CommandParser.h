#pragma once
#include <stdint.h>

#include <Collection.h>
#include <FixedArray.h>
#include <Slice.h>

#include "Commands.h"
#include "CommandBuilder.h"

extern Serial serial;

template <class CommandHandlerT>
class CommandParser : public CommandHandlerT
{
public:
    enum class ParserState : uint8_t
    {
        Idle,
        Command_Node,
        Command_Device,
        Command_Message,
        Parameters,
        Complete,
        Error
    };

    enum class ParserError : uint8_t
    {
        NoError,
        InvalidCommand,
        InvalidParameter
    };

    CommandParser()
        : _slice(_params.getBuffer(), _params.getCapacity())
    {
    }

    bool Parse(uint8_t data)
    {
        switch (_state)
        {
        case ParserState::Idle:
            if (data == Command::EOM)
            {
                // ignore
                return false;
            }

            _command.NodeId = data;
            _state = ParserState::Command_Node;
            // serial.Transmit.Write("N:");
            // serial.Transmit.WriteLine(data);
            return true;
        case ParserState::Command_Node:
            if (data == Command::EOM)
            {
                _state = ParserState::Error;
                _error = ParserError::InvalidCommand;
                // serial.Transmit.WriteLine("E0");
                return true;
            }
            _command.DeviceId = data;
            _state = ParserState::Command_Device;
            // serial.Transmit.Write("D:");
            // serial.Transmit.WriteLine(data);
            break;
        case ParserState::Command_Device:
            if (data == Command::EOM)
            {
                _state = ParserState::Error;
                _error = ParserError::InvalidCommand;
                // serial.Transmit.WriteLine("E1");
                return true;
            }
            _command.MessageId = data;
            _state = ParserState::Command_Message;
            // serial.Transmit.Write("M:");
            // serial.Transmit.WriteLine(data);
            break;
        case ParserState::Command_Message:
            if (data == Command::EOM)
            {
                _state = ParserState::Complete;
                // serial.Transmit.WriteLine("C0");
                return true;
            }
            _params.Add(data);
            _state = ParserState::Parameters;
            // serial.Transmit.Write("P0:");
            // serial.Transmit.WriteLine(data);
            return true;
        case ParserState::Parameters:
            if (data == Command::EOM)
            {
                _state = ParserState::Complete;
                // serial.Transmit.WriteLine("C1");
                return true;
            }
            _params.Add(data);
            // serial.Transmit.Write("P1:");
            // serial.Transmit.WriteLine(data);
            return true;
        default:
            //_state = ParserState::Idle;
            break;
        }

        return false;
    }

    bool Dispatch()
    {
        // build typed command
        Command *typedCommand = nullptr;
        if (!TryBuildTypedCommand(_command, _slice, &typedCommand))
        {
            // serial.Transmit.WriteLine("TypedCmd Failed");
            return false;
        }
        // global commands
        if (OnGlobalCommand<GlobalResetCommand>(typedCommand, GlobalMessages::Reset))
            return true;

        // node commands
        // if (OnNodeCommand<NodeResetCommand>(typedCommand, NodeMessages::Reset))
        //     return true;
        if (OnNodeCommand<BlockPowerCommand>(typedCommand, NodeMessages::BlockPower))
            return true;
        if (OnNodeCommand<BlockSpeedCommand>(typedCommand, NodeMessages::BlockSpeed))
            return true;

        // device commands
        // if (OnDeviceCommand(typedCommand, DeviceMessages::Reset))
        //     return true;

        // serial.Transmit.WriteLine("No Dispatch");
        return false;
    }

    void Clear()
    {
        _state = ParserState::Idle;
        _error = ParserError::NoError;
        _command.NodeId = Command::NoneId;
        _command.DeviceId = Command::NoneId;
        _command.MessageId = Command::NoneId;
        _params.Clear();
    }

    bool IsError() const
    {
        return _state == ParserState::Error;
    }

    ParserError getError() const
    {
        return _error;
    }

    bool IsComplete() const
    {
        return _state == ParserState::Complete;
    }

private:
    ParserState _state = ParserState::Idle;
    ParserError _error = ParserError::NoError;
    Command _command;
    Collection<FixedArray<uint8_t, 8>> _params;
    CommandBuffer _slice;

    template <typename CommandT>
    bool OnGlobalCommand(Command *typedCommand, GlobalMessages message)
    {
        if (_command.IsGlobalMessage(message) &&
            CommandT::IsValidMessage(message))
        {
            CommandHandlerT::OnCommand(static_cast<CommandT *>(typedCommand));
            return true;
        }
        return false;
    }

    template <typename CommandT>
    bool OnNodeCommand(Command *typedCommand, NodeMessages message)
    {
        if (_command.IsNodeMessage(message) &&
            CommandT::IsValidMessage(message))
        {
            CommandHandlerT::OnCommand(static_cast<CommandT *>(typedCommand));
            return true;
        }
        return false;
    }

    template <typename CommandT>
    bool OnDeviceCommand(Command *typedCommand, DeviceMessages message)
    {
        if (_command.IsDeviceMessage(message) &&
            CommandT::IsValidMessage(message))
        {
            CommandHandlerT::OnCommand(static_cast<CommandT *>(typedCommand));
            return true;
        }
        return false;
    }
};