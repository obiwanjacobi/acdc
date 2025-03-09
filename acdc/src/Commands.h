#pragma once
#include <stdint.h>
#include "../lib/atl/FixedArray.h"
#include "../lib/atl/Debug.h"

enum class ParserError : uint8_t
{
    NoError,
    InvalidCommand,
    InvalidParameter
};

enum class CommandType : uint8_t
{
    None,
    Power,
    Status,
    TrackMode
};

template <class BaseT>
class CommandDispatcher : public BaseT
{
public:
    bool Dispatch(CommandType command, const uint8_t *params, uint8_t count)
    {
        switch (command)
        {
        case CommandType::Power:
            Trace("Dispatch: OnPower");
            BaseT::OnPower(params[0] == 1, params[1]);
            return true;
        default:
            return false;
        }
    }

private:
    void Trace(const char *message)
    {
        Debug<2>::Log<DebugLevel::Trace>(message);
    }
};

// -----------------------------------------------------------------------------

template <class BufferT>
class CommandParser
{
public:
    enum class ParserState : uint8_t
    {
        Idle,
        Command,
        Parameter,
        Complete,
        Error
    };

    CommandParser()
    {
        Clear();
    }

    template <class T>
    bool Dispatch(CommandDispatcher<T> &dispatcher)
    {
        if (IsComplete())
        {
            return dispatcher.Dispatch(_command, _params.getBuffer(), _paramIndex);
        }
        return false;
    }

    bool Parse(char data)
    {
        switch (_state)
        {
        case ParserState::Complete:
        case ParserState::Error:
        case ParserState::Idle:
            if (data == '<')
            {
                Clear();
                Trace("State: Idle->Command");
                _state = ParserState::Command;
                return true;
            }
            if (data == '>')
            {
                Trace("State: Idle->Error[InvalidCommand]");
                _error = ParserError::InvalidCommand;
                return true;
            }
            break;
        case ParserState::Command:
            if (data == '>')
            {
                if (ParseCommand())
                {
                    Trace("State: Command->Complete");
                    _state = ParserState::Complete;
                }
                else
                {
                    Trace("State: Command->Error[InvalidCommand]");
                    _error = ParserError::InvalidCommand;
                    _state = ParserState::Error;
                }
                return true;
            }
            else if (data == ' ')
            {
                if (ParseCommand())
                {
                    Trace("State: Command->Parameter");
                    _state = ParserState::Parameter;
                }
                else
                {
                    Trace("State: Command->Error[InvalidCommand]");
                    _error = ParserError::InvalidCommand;
                    _state = ParserState::Error;
                }
                return true;
            }
            else
            {
                _buffer.Write(data);
                return true;
            }
            break;
        case ParserState::Parameter:
            if (data == '>')
            {
                Trace("State: Parameter->Complete");
                _state = ParserState::Complete;
                return true;
            }
            else if (data == ' ')
            {
                if (!ParseParameter())
                {
                    Trace("State: Parameter->Error[InvalidParameter]");
                    _error = ParserError::InvalidParameter;
                    _state = ParserState::Error;
                }
                return true;
            }
            else
            {
                _buffer.Write(data);
                return true;
            }
            break;
        }

        return false;
    }

    void Clear()
    {
        _state = ParserState::Idle;
        _command = CommandType::None;
        _error = ParserError::NoError;
        _paramIndex = 0;
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
    BufferT _buffer;
    ParserState _state = ParserState::Idle;
    CommandType _command = CommandType::None;
    ParserError _error = ParserError::NoError;
    uint8_t _paramIndex = 0;
    FixedArray<uint8_t, 8> _params;

    /**
     * @brief Parse the command from the buffer.
     * /return Returns true if the command is valid.
     */
    bool ParseCommand()
    {
        char data;
        if (_buffer.TryRead(&data))
        {
            switch (data)
            {
            case '0':
            case '1':
                _command = CommandType::Power;
                _params[_paramIndex++] = data - '0';
                return true;
            case '=':
                _command = CommandType::TrackMode;
                return true;
            case 's':
                _command = CommandType::Status;
                return true;
            default:
                // TODO: continue reading multi char commands
                return false;
            }
        }
        return false;
    }

    bool ParseParameter()
    {
#define PARAM_LEN 8
        char data[PARAM_LEN];
        uint16_t count = _buffer.Read(data, PARAM_LEN);

        if (count > 0)
        {
            if (count == 1)
            {
                _params[_paramIndex++] = data[0];
            }
            else
            {
                // TODO: parse multi char parameters
                _params[_paramIndex++] = data[0];
            }
            return true;
        }

        return false;
    }

    void Trace(const char *message)
    {
        Debug<1>::Log<DebugLevel::Trace>(message);
    }
};
