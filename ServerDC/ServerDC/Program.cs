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

var parser = new Parser();
var notificationSvc = app.Services.GetRequiredService<NotificationService>();
var comService = app.Services.GetRequiredService<ComDC>();
comService.Open("COM3");

while (true)
{
    while (Console.KeyAvailable)
    {
        var k = Console.ReadKey().KeyChar;
        parser.Parse(k);
        if (k == '\r') Console.WriteLine(".");
    }
    if (parser.IsComplete)
    {
        //Console.WriteLine("sending...");
        comService.Send(parser.CommandData);
        parser.Clear();
    }
    if (parser.IsError)
    {
        Console.WriteLine("Error... Try again");
        parser.Clear();
    }

    if (comService.HasIncoming)
    {
        if (comService.TryReceive(out byte[]? msg))
        {
            if (msg.Length > 0)
            {
                for (int i = 0; i < msg.Length; i++)
                {
                    if (msg[i] == 0xFF)
                    {
                        Console.WriteLine();
                        continue;
                    }
                    else if (i > 0)
                        Console.Write(",");

                    Console.Write(msg[i]);
                }

                Console.WriteLine();

                //Console.WriteLine(System.Text.Encoding.ASCII.GetChars(msg));
            }
        }
    }

    //await notificationSvc.OnMessage(line ?? "<Empty>");

    await Task.Delay(10);
}
