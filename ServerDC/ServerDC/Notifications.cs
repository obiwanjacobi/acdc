using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.SignalR;

namespace ServerDC;

public interface IOperationNotifications
{
    Task OnTurnoutChanged(int turnoutId, int prevState, int newState);
    Task OnSignalChanged(int signalId, int prevState, int newState);
    Task OnBlockOccupationChanged(int blockId, int prevState, int newState);
    Task OnMessage(string message);
}

public sealed class NotificationHub : Hub<IOperationNotifications>
{
    public Task Subscribe([FromServices] INotificationUsers notificationUsers)
    {
        // Use Context.ConnectionId to identify client
        return notificationUsers.Subscribe(Context.ConnectionId);
    }
}

public interface INotificationUsers
{
    Task Subscribe(string connectionId /* any other subscription identifying params */);
}

internal sealed class NotificationService(IHubContext<NotificationHub, IOperationNotifications> hubContext) : IOperationNotifications, INotificationUsers
{
    public Task OnTurnoutChanged(int turnoutId, int prevState, int newState)
    {
        return hubContext.Clients.All.OnTurnoutChanged(turnoutId, prevState, newState);
    }

    public Task OnSignalChanged(int signalId, int prevState, int newState)
    {
        return hubContext.Clients.All.OnSignalChanged(signalId, prevState, newState);
    }

    public Task OnBlockOccupationChanged(int blockId, int prevState, int newState)
    {
        return hubContext.Clients.All.OnBlockOccupationChanged(blockId, prevState, newState);
    }

    public Task OnMessage(string message)
    {
        return hubContext.Clients.All.OnMessage(message);
    }

    public Task Subscribe(string connectionId /* other filtering data */)
    {
        //hubContext.Groups.AddToGroupAsync
        return Task.CompletedTask;
    }
}

internal static class NotificationExtensions
{
    public static IServiceCollection AddNotifications(this IServiceCollection services)
    {
        services.AddSignalR();

        // notification service
        services.AddSingleton<NotificationService>();
        services.AddSingleton<IOperationNotifications>(serviceProvider
            => serviceProvider.GetRequiredService<NotificationService>());
        services.AddSingleton<INotificationUsers>(serviceProvider
            => serviceProvider.GetRequiredService<NotificationService>());

        return services;
    }

    public static void MapNotifications(this WebApplication app)
    {
        app.MapHub<NotificationHub>("/notifications");
    }
}