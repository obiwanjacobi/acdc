#pragma once
#include <stdint.h>

#include <BitArray.h>
#include <Slice.h>

typedef Slice<uint8_t> CommandBuffer;

enum class GlobalMessages : uint8_t
{
    None = 0x00,
    Reset = 0x01, // resets all nodes on the network

    Invalid = 0xFF
};

enum class NodeMessages : uint8_t
{
    None = 0x00,
    Reset = 0x01, // resets the node and all its devices

    BlockPower = 0x40,      // turns power on/off for a specified block
    BlockSpeed = 0x41,      // sets speed for a specified block
    BlockOccupation = 0x42, // event

    Invalid = 0xFF
};

enum class DeviceMessages : uint8_t
{
    None = 0x00,
    Reset = 0x01, // resets (re-init) the device

    Invalid = 0xFF
};

struct Command
{
    static const uint8_t NoneId = 0x00;
    static const uint8_t InvalidId = 0xFF;
    // End of Message
    static const uint8_t EOM = 0xFF;

    uint8_t NodeId;
    uint8_t DeviceId;
    uint8_t MessageId;

    bool IsValidCommand()
    {
        return IsGlobalCommand() || IsNodeCommand() || IsDeviceCommand();
    }

    bool IsGlobalCommand()
    {
        return NodeId == NoneId && DeviceId == NoneId;
    };

    bool IsGlobalMessage(GlobalMessages message)
    {
        return IsGlobalCommand() && IsMessage(message);
    };

    bool IsNodeCommand()
    {
        return NodeId != NoneId && NodeId != InvalidId && DeviceId == NoneId;
    };

    bool IsNodeMessage(NodeMessages message)
    {
        return IsNodeCommand() && IsMessage(message);
    };

    bool IsDeviceCommand()
    {
        return NodeId != NoneId && NodeId != InvalidId && DeviceId != NoneId && DeviceId != InvalidId;
    };

    bool IsDeviceMessage(DeviceMessages message)
    {
        return IsDeviceCommand() && IsMessage(message);
    };

    void Take(const Command &command)
    {
        NodeId = command.NodeId;
        DeviceId = command.DeviceId;
        MessageId = command.MessageId;
    }

protected:
    uint8_t Serialize(CommandBuffer &buffer)
    {
        uint8_t index = 0;
        buffer[index++] = NodeId;
        buffer[index++] = DeviceId;
        buffer[index++] = MessageId;
        return index;
    }

private:
    template <typename MessageEnumT>
    bool IsMessage(MessageEnumT message)
    {
        return MessageId == static_cast<uint8_t>(message);
    }
};

//-----------------------------------------------------------------------------
struct GlobalResetCommand;

struct GlobalCommand : Command
{
    static bool TryCreate(Command &command, CommandBuffer &data, GlobalCommand **outCommand)
    {
        if (TryCreate<GlobalResetCommand>(command, data, outCommand))
            return true;

        *outCommand = nullptr;
        return false;
    }

protected:
    GlobalCommand() {}

private:
    template <typename GlobalCommandT>
    static bool TryCreate(Command command, CommandBuffer &data, GlobalCommand **outCommand)
    {
        return GlobalCommandT::TryCreate(command, data, (GlobalCommandT **)outCommand);
    }
};

struct GlobalResetCommand : GlobalCommand
{
    static bool TryCreate(Command &command, CommandBuffer &data, GlobalResetCommand **outCommand)
    {
        if (command.IsGlobalMessage(GlobalMessages::Reset))
        {
            _instance.Take(command);
            *outCommand = &_instance;
            return true;
        }

        *outCommand = nullptr;
        return false;
    }

    static bool IsValidMessage(GlobalMessages message)
    {
        return message == GlobalMessages::Reset;
    }

private:
    static GlobalResetCommand _instance;
};

GlobalResetCommand GlobalResetCommand::_instance;

//-----------------------------------------------------------------------------
struct BlockPowerCommand;
struct BlockSpeedCommand;

struct NodeCommand : Command
{
    static bool TryCreate(Command &command, CommandBuffer &data, NodeCommand **outCommand)
    {
        if (TryCreate<BlockPowerCommand>(command, data, outCommand))
            return true;
        if (TryCreate<BlockSpeedCommand>(command, data, outCommand))
            return true;

        *outCommand = nullptr;
        return false;
    }

protected:
    NodeCommand() {}

private:
    template <typename NodeCommandT>
    static bool TryCreate(Command command, CommandBuffer &data, NodeCommand **outCommand)
    {
        return NodeCommandT::TryCreate(command, data, (NodeCommandT **)outCommand);
    }
};

struct BlockNodeCommand : NodeCommand
{
    uint8_t BlockId;

protected:
    uint8_t Serialize(CommandBuffer &buffer)
    {
        uint8_t index = NodeCommand::Serialize(buffer);
        buffer[index++] = BlockId;
        return index;
    }
};

struct BlockPowerCommand : BlockNodeCommand
{
    bool PowerOn;

    static bool TryCreate(Command command, CommandBuffer &data, BlockPowerCommand **outCommand)
    {
        if (command.IsNodeMessage(NodeMessages::BlockPower))
        {
            _instance.Take(command);
            _instance.BlockId = data[0];
            _instance.PowerOn = data[1] > 0;
            *outCommand = &_instance;
            return true;
        }

        *outCommand = nullptr;
        return false;
    }

    static bool IsValidMessage(NodeMessages message)
    {
        return message == NodeMessages::BlockPower;
    }

private:
    static BlockPowerCommand _instance;
};

BlockPowerCommand BlockPowerCommand::_instance;

struct BlockSpeedCommand : BlockNodeCommand
{
    uint8_t Speed;

    static bool TryCreate(Command command, CommandBuffer &data, BlockSpeedCommand **outCommand)
    {
        if (command.IsNodeMessage(NodeMessages::BlockSpeed))
        {
            _instance.Take(command);
            _instance.BlockId = data[0];
            _instance.Speed = data[1];
            *outCommand = &_instance;
            return true;
        }

        *outCommand = nullptr;
        return false;
    }

    static bool IsValidMessage(NodeMessages message)
    {
        return message == NodeMessages::BlockSpeed;
    }

private:
    static BlockSpeedCommand _instance;
};

BlockSpeedCommand BlockSpeedCommand::_instance;

struct BlockOccupationEvent : public NodeCommand
{
    // max 8 flags
    BitArray<uint8_t> OccupationFlags;

    static BlockOccupationEvent *Create(uint8_t nodeId, BitArray<uint8_t> occupationFlags)
    {
        _instance.NodeId = nodeId;
        _instance.DeviceId = NoneId;
        _instance.MessageId = static_cast<uint8_t>(NodeMessages::BlockOccupation);
        _instance.OccupationFlags.SetAll(occupationFlags.GetAll());

        return &_instance;
    }

    uint8_t Serialize(CommandBuffer &buffer)
    {
        uint8_t index = NodeCommand::Serialize(buffer);
        buffer[index++] = OccupationFlags.GetAll();
        buffer[index++] = EOM;
        return index;
    }

private:
    static BlockOccupationEvent _instance;
};

BlockOccupationEvent BlockOccupationEvent::_instance;

//-----------------------------------------------------------------------------

struct DeviceCommand : Command
{
    static bool TryCreate(Command &command, CommandBuffer &data, NodeCommand **outCommand)
    {
        // if (TryCreate<ResetDeviceCommand>(command, data, outCommand))
        //     return true;

        *outCommand = nullptr;
        return false;
    }

protected:
    DeviceCommand() {}

private:
    template <typename DeviceCommandT>
    static bool TryCreate(Command command, CommandBuffer &data, NodeCommand **outCommand)
    {
        return DeviceCommandT::TryCreate(command, data, (DeviceCommandT **)outCommand);
    }
};

//-----------------------------------------------------------------------------
