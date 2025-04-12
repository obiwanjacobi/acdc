
using System.Collections.Concurrent;

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

    private readonly ConcurrentDictionary<int, Action<string>> _subscribers = new();

    public IDisposable Subscribe(Action<string> subscriber)
    {
        var id = _subscribers.Keys.Aggregate((acc, val) => Math.Max(acc, val));
        id++;

        _subscribers.AddOrUpdate(id, subscriber, (id, sub) => sub);
        return new SubscriptionScope(_subscribers, id);
    }

    private void Publish(string message)
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
        while (SerialCom.TryReceiveMessage(out string? msg))
        {
            Publish(msg);
        }
    }

    public void StopNotificationTimer()
    {
        _notificationTimer?.Dispose();
        _notificationTimer = null;
    }

    private sealed class SubscriptionScope(IDictionary<int, Action<string>> subscribers, int id) : IDisposable
    {
        public void Dispose()
        {
            subscribers.Remove(id);
        }
    }
}
