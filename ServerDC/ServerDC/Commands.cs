namespace ServerDC;

public class Command
{
    public byte NodeId
    {
        get { return Data[0]; }
        set { Data[0] = value; }
    }
    public byte DeviceId
    {
        get { return Data[1]; }
        set { Data[1] = value; }
    }
    public byte MessageId
    {
        get { return Data[2]; }
        set { Data[2] = value; }
    }

    public required List<byte> Data { get; set; }
}
