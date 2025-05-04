
using System.Collections.Concurrent;
using System.Diagnostics.CodeAnalysis;

namespace ServerDC;

internal sealed partial class ComDC
{
    public bool IsOpen => SerialCom.IsOpen;

    public void Open(string comPort)
    {
        SerialCom.Open(comPort, 115200);
        StartNotificationTimer();
    }

    public void Close()
    {
        StopNotificationTimer();
        SerialCom.Close();
    }

    public void Send(byte[] message)
    {
        SerialCom.SendMessage(message);
    }

    public bool HasIncoming
    {
        get { return SerialCom.IncomingMessageCount > 0; }
    }

    public bool TryReceive([NotNullWhen(true)] out byte[]? message)
    {
        return SerialCom.TryReceiveMessage(out message);
    }

    private readonly ConcurrentDictionary<int, Action<byte[]>> _subscribers = new();

    public IDisposable Subscribe(Action<byte[]> subscriber)
    {
        var id = _subscribers.Keys.Aggregate((acc, val) => Math.Max(acc, val));
        id++;

        _subscribers.AddOrUpdate(id, subscriber, (id, sub) => sub);
        return new SubscriptionScope(_subscribers, id);
    }

    private void Publish(byte[] message)
    {
        foreach (var subscriber in _subscribers)
        {
            try
            {
                subscriber.Value(message);
            }
            catch (Exception ex)
            {
                // TODO: log exception
            }
        }
    }
    private Timer? _notificationTimer;

    private void StartNotificationTimer()
    {
        _notificationTimer = new Timer(NotificationCallback, null, 0, 200);
    }

    private void NotificationCallback(object? _)
    {
        while (SerialCom.TryReceiveMessage(out byte[]? msg))
        {
            Publish(msg);
        }
    }

    public void StopNotificationTimer()
    {
        _notificationTimer?.Dispose();
        _notificationTimer = null;
    }

    internal void Send(object value)
    {
        throw new NotImplementedException();
    }

    private sealed class SubscriptionScope(IDictionary<int, Action<byte[]>> subscribers, int id) : IDisposable
    {
        public void Dispose()
        {
            subscribers.Remove(id);
        }
    }
}
