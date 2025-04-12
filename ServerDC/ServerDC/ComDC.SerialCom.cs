using System.Diagnostics.CodeAnalysis;
using System.IO.Ports;

namespace ServerDC;

internal sealed partial class ComDC
{
    private static class SerialCom
    {
        private static Lock _lock = new();
        private static Queue<string> _incoming = new();
        private static Queue<string> _outgoing = new();
        // do not require locks
        private static SerialPort _serialPort = new();
        private static Thread? _backgroundThread;
        private static volatile bool _running = false;
        private static string _partial = String.Empty;

        public static void Open(string comPort, int baudRate)
        {
            Close();
            _serialPort.PortName = comPort;
            _serialPort.BaudRate = baudRate;
            _serialPort.Open();
            Start();
        }

        public static void Close()
        {
            if (_serialPort.IsOpen)
            {
                Stop();
                _serialPort.Close();
            }
        }

        public static bool IsOpen => _serialPort.IsOpen;

        public static int IncomingMessageCount
        {
            get
            {
                int count = 0;
                lock (_lock)
                {
                    count = _incoming.Count;
                }

                return count;
            }
        }

        public static int OutgoingMessageCount
        {
            get
            {
                int count = 0;
                lock (_lock)
                {
                    count = _outgoing.Count;
                }

                return count;
            }
        }

        public static bool TryReceiveMessage([NotNullWhen(true)] out string? outMessage)
        {
            lock (_lock)
            {
                if (_incoming.Count > 0)
                {
                    outMessage = _incoming.Dequeue();
                    return true;
                }
            }

            outMessage = null;
            return false;
        }

        public static void SendMessage(string message)
        {
            lock (_lock)
            {
                _outgoing.Enqueue(message);
            }
        }

        private static void Start()
        {
            Stop();

            _running = true;
            _backgroundThread = new Thread(Run);
            _backgroundThread.IsBackground = true;
            _backgroundThread.Start();
        }

        private static void Stop()
        {
            if (_backgroundThread is not null)
            {
                _running = false;
                _backgroundThread.Join();
                _backgroundThread = null;
            }
        }

        private static void Run(object? obj)
        {
            while (_running)
            {
                try
                {
                    while (Receive() || Send()) ;

                    // SerialPort.DataReceived is very unreliable
                    // so we're back to polling
                    Thread.Sleep(100);
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Error in Serial Background Thread:");
                    Console.WriteLine(ex.ToString());
                }
            }
        }

        private static bool Receive()
        {
            var buffer = _serialPort.ReadExisting();
            if (!String.IsNullOrEmpty(buffer))
            {
                Parse(buffer);
                return true;
            }

            return false;
        }

        private static bool Send()
        {
            bool hasWork = false;
            string? msg = null;

            lock (_lock)
            {
                if (_outgoing.Count > 0)
                    msg = _outgoing.Dequeue();

                hasWork = _outgoing.Count > 0;
            }

            if (msg is not null)
                _serialPort.WriteLine(msg);

            return hasWork;
        }

        // End Of Message
        const char EOM = '\n';

        private static void Parse(string partial)
        {
            var parts = partial.Split(EOM);

            int i = 0;
            // partial left over from the last time?
            if (_partial.Length > 0)
            {
                var msg = _partial + parts[i++];
                _partial = String.Empty;

                lock (_lock)
                {
                    _incoming.Enqueue(msg);
                }
            }

            // Process all complete messages (all parts except the last one)
            for (; i < parts.Length - 1; i++)
            {
                var part = parts[i];
                // empty messages are skipped
                if (String.IsNullOrEmpty(part)) continue;

                lock (_lock)
                {
                    _incoming.Enqueue(part);
                }
            }

            // Handle the last part (could be a partial message)
            _partial += parts[^1];

            // If the last part ends with EOM, it's a complete message
            if (partial.EndsWith(EOM))
            {
                lock (_lock)
                {
                    _incoming.Enqueue(_partial);
                }
                _partial = String.Empty;
            }
        }
    }
}
