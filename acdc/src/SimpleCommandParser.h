#pragma once
#include <stdint.h>

// BaseT is the class that will handle/implement the actual commands

template <class BaseT>
class CommandParser : public BaseT
{
public:
    enum class CommandType : uint8_t
    {
        None,
        Power,     //'Po' or 'P' (off)
        Speed,     //'Sn' (n=0-9)
        Direction, //'Df' or 'Db'
    };

    enum class ParserState : uint8_t
    {
        Idle,
        Command,
        Parameter,
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
    {
        Clear();
    }

    bool Parse(char data)
    {
        switch (_state)
        {
        case ParserState::Complete:
        case ParserState::Error:
        case ParserState::Idle:
            if (data == 'P')
            {
                _command = CommandType::Power;
                _state = ParserState::Command;
                return true;
            }
            if (data == 'S')
            {
                _command = CommandType::Speed;
                _state = ParserState::Command;
                return true;
            }
            if (data == 'D')
            {
                _command = CommandType::Direction;
                _state = ParserState::Command;
                return true;
            }
            Clear();
            return false;

        case ParserState::Command:
            if (data == ' ')
            {
                // ignore, but no error
                return true;
            }
            if (data == 'f' || data == 'b')
            {
                if (_command != CommandType::Direction)
                {
                    _error = ParserError::InvalidParameter;
                    return false;
                }
                _params[0] = data;
                _state = ParserState::Parameter;
                return true;
            }
            if (data >= '0' && data <= '9')
            {
                if (_command != CommandType::Speed)
                {
                    _error = ParserError::InvalidParameter;
                    return false;
                }
                _params[0] = data - '0';
                _state = ParserState::Parameter;
                return true;
            }
            if (data == 'o')
            {
                if (_command != CommandType::Power)
                {
                    _error = ParserError::InvalidParameter;
                    return false;
                }
                _params[0] = data;
                _state = ParserState::Parameter;
                return true;
            }
            // no param
            else if (data == '\n' &&
                     _command == CommandType::Power)
            {
                _state = ParserState::Complete;
                return true;
            }

            _state = ParserState::Error;
            _error = ParserError::InvalidParameter;
            return false;

        case ParserState::Parameter:
            if (data == '\n' &&
                _state != ParserState::Error)
            {
                _state = ParserState::Complete;
                return true;
            }
            return false;

        default:
            return false;
        }
    }

    bool Dispatch()
    {
        if (_state != ParserState::Complete)
            return false;

        switch (_command)
        {
        case CommandType::Power:
            BaseT::OnPower(_params[0] == 'o');
            return true;
        case CommandType::Speed:
            BaseT::OnSpeed(_params[0]);
            return true;
        case CommandType::Direction:
            BaseT::OnDirection(_params[0] == 'f');
            return true;
        default:
            return false;
        }
    }

    void Clear()
    {
        _state = ParserState::Idle;
        _command = CommandType::None;
        _error = ParserError::NoError;
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
    CommandType _command = CommandType::None;
    ParserError _error = ParserError::NoError;
    FixedArray<uint8_t, 1> _params;
};
