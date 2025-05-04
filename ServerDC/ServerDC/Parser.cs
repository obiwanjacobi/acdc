namespace ServerDC;

public class Parser
{
    // BnSm Block <n> Speed <m>

    public bool IsComplete { get; internal set; }
    public bool IsError => _state < 0;

    private int _state = 0;
    private string _blockId = String.Empty;
    private string _speed = String.Empty;

    public void Parse(char data)
    {
        if (data == ' ') return;
        if (data == '\r') data = '\n';
        if (data == '\t') return;
        if (data == 'x')
        {
            Clear();
            return;
        }

        switch (_state)
        {
            case 0:
                if (data == 'b')
                {
                    _state++;
                }
                break;
            case 1:
                if (Char.IsAsciiDigit(data))
                    _blockId += data;
                else if (data == 's')
                {
                    if (!Byte.TryParse(_blockId, out var val))
                        _state = -1;
                    else
                        _state++;
                }
                else
                    _state = -1;    // error
                break;
            case 2:
                if (Char.IsAsciiDigit(data))
                    _speed += data;
                else if (data == '\n')
                {
                    if (!Byte.TryParse(_speed, out var val))
                    {
                        _state = -1;
                    }
                    else
                    {
                        _state = 0;
                        IsComplete = true;
                    }
                }
                else
                    _state = -1;    // error
                break;
        }
    }

    public byte[] CommandData
    {
        get
        {
            var block = Byte.Parse(_blockId);
            var speed = Byte.Parse(_speed);
            List<Byte> buffer = [];

            if (block == 0)
            {
                AddMessage(buffer, 1, speed);
                AddMessage(buffer, 2, speed);
                AddMessage(buffer, 3, speed);
                AddMessage(buffer, 4, speed);
            }
            else
            {
                AddMessage(buffer, block, speed);
            }

            return buffer.ToArray();
        }
    }

    public void Clear()
    {
        _state = 0;
        IsComplete = false;
        _blockId = String.Empty;
        _speed = String.Empty;
    }

    private void AddMessage(List<byte> buffer, byte blockId, byte speed)
    {
        // NodeId = 1
        // DeviceId = 0
        // Msg = 0x41 => Node-BlockSpeed
        // 0xFF = End of Message
        buffer.AddRange([1, 0, 0x41, blockId, speed, 0xFF]);
    }
}
