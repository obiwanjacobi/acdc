using FastEndpoints;
using ServerDC;

var builder = WebApplication.CreateSlimBuilder(args);
var services = builder.Services;
//var configuration = ;

services.AddFastEndpoints();
services.AddSingleton<ComDC>();
services.AddNotifications();
services.AddCors();

var app = builder.Build();

// cors for signalR client connection
app.UseCors(options =>
{
    options
        .WithOrigins("http://localhost:1420")
        .AllowAnyHeader()
        .AllowAnyMethod()
        .AllowCredentials()
        ;
});

app.UseFastEndpoints();
app.MapNotifications();
// ping
app.MapGet("/", () => "ok");

var runTask = app.RunAsync();

var notificationSvc = app.Services.GetRequiredService<NotificationService>();
while (true)
{
    var line = Console.ReadLine();
    await notificationSvc.OnMessage(line ?? "<Empty>");
}
