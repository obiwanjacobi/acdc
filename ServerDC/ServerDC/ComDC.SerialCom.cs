using System.Diagnostics.CodeAnalysis;
using System.IO.Ports;

namespace ServerDC;

internal sealed partial class ComDC
{
    private static class SerialCom
    {
        private static Lock _lock = new();
        private static Queue<byte[]> _incoming = new();
        private static Queue<byte[]> _outgoing = new();
        // do not require locks
        private static SerialPort _serialPort = new();
        private static Thread? _backgroundThread;
        private static volatile bool _running = false;
        private static List<byte> _partial = [];

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
                lock (_lock)
                {
                    return _incoming.Count;
                }
            }
        }

        public static int OutgoingMessageCount
        {
            get
            {
                lock (_lock)
                {
                    return _outgoing.Count;
                }
            }
        }

        public static bool TryReceiveMessage([NotNullWhen(true)] out byte[]? outMessage)
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

        public static void SendMessage(byte[] message)
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
            var bytesToRead = _serialPort.BytesToRead;
            if (bytesToRead == 0) return false;

            var buffer = new byte[bytesToRead];
            var read = _serialPort.Read(buffer, 0, bytesToRead);

            if (read > 0)
            {
                Parse(buffer, Math.Min(bytesToRead, read));
                //Console.WriteLine(Encoding.ASCII.GetChars(buffer));
                return true;
            }

            return false;
        }

        private static bool Send()
        {
            bool hasWork = false;
            byte[]? msg = null;

            lock (_lock)
            {
                if (_outgoing.Count > 0)
                    msg = _outgoing.Dequeue();

                hasWork = _outgoing.Count > 0;
            }

            if (msg is not null)
                _serialPort.Write(msg, 0, msg.Length);

            return hasWork;
        }

        // End Of Message
        const byte EOM = 0xFF;

        private static void Parse(byte[] buffer, int count)
        {
            lock (_lock)
            {
                _incoming.Enqueue(buffer);
            }

            //for (int i = 0; i < count; i++)
            //{
            //    var data = buffer[i];

            //    if (data == EOM)
            //    {
            //        var arr = _partial.ToArray();
            //        _partial.Clear();

            //        lock (_lock)
            //        {
            //            _incoming.Enqueue(arr);
            //        }
            //    }
            //    else
            //        _partial.Add(data);
            //}
        }
    }
}
