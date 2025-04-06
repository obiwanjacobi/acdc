import { useEffect, useState } from "react";
import "./App.css";
import Button from "@mui/material/Button";

function App() {
    const [port, setPort] = useState<SerialPort | null>(null);
    const [serialOutput, setSerialOutput] = useState<string>("");
    const [inputText, setInputText] = useState<string>("");
    const [isConnected, setIsConnected] = useState<boolean>(false);
    const [disconnect, setDisconnect] = useState<boolean>(false);

    // Connect to a serial port
    async function connectToSerial() {
        if (!navigator.serial) {
            alert("Web Serial API not supported in this browser!");
            return;
        }

        try {
            // Request a port and open it
            const port = await navigator.serial.requestPort();
            await port.open({ baudRate: 115200 });
            setPort(port);
            setIsConnected(true);
            setDisconnect(false);

            readSerialData(port);
        } catch (error) {
            console.error("Error connecting to serial port:", error);
        }
    }

    function startDisconnect() {
        setDisconnect(true);
    }

    // Disconnect from serial port
    useEffect(() => {
        async function disconnectSerial() {
            if (port && disconnect) {
                try {
                    await port.close();
                    setPort(null);
                    setIsConnected(false);
                    console.log("Disconnected from serial port");
                } catch (error) {
                    console.error("Error disconnecting:", error);
                }
            }
        }

        disconnectSerial();
    }, [disconnect]);

    // Clean up on component unmount
    useEffect(() => {
        return () => {
            if (port) {
                port.close().catch(console.error);
            }
        };
    }, [port]);

    // Read data from the serial port
    async function readSerialData(port: SerialPort) {
        if (!port.readable) return;

        const reader = port.readable.getReader();
        const decoder = new TextDecoder();

        try {
            while (!disconnect) {
                const { value, done } = await reader.read();
                if (done) break;

                const text = decoder.decode(value);
                setSerialOutput((prev) => prev + text);
            }
        } catch (error) {
            console.error("Error reading from serial port:", error);
        } finally {
            reader.releaseLock();
        }
    }

    // Write data to the serial port
    async function writeToSerial() {
        if (!port || !port.writable || !inputText) return;

        const writer = port.writable.getWriter();
        const encoder = new TextEncoder();

        try {
            await writer.write(encoder.encode(inputText + "\n"));
            setInputText("");
        } catch (error) {
            console.error("Error writing to serial port:", error);
        } finally {
            writer.releaseLock();
        }
    }

    return (
        <main className="container">
            <div
                className="serial-section"
                style={{
                    marginTop: "2rem",
                    padding: "1rem",
                    border: "1px solid #ccc",
                    borderRadius: "8px",
                }}
            >
                <h2>Run DC / Automatic Control DC</h2>

                <div className="row" style={{ marginBottom: "1rem" }}>
                    <Button onClick={connectToSerial} disabled={isConnected}>
                        Connect to Serial
                    </Button>
                    <Button onClick={startDisconnect} disabled={!isConnected}>
                        Disconnect
                    </Button>
                </div>

                {isConnected && (
                    <>
                        <div style={{ marginBottom: "1rem" }}>
                            <h3>Serial Output:</h3>
                            <pre
                                style={{
                                    padding: "0.5rem",
                                    height: "100px",
                                    overflowY: "scroll",
                                }}
                            >
                                {serialOutput || "No data received yet..."}
                            </pre>
                        </div>

                        <div className="row">
                            <input
                                value={inputText}
                                onChange={(e) => setInputText(e.currentTarget.value)}
                                placeholder="Text to send..."
                            />
                            <Button onClick={writeToSerial} disabled={inputText.length == 0}>
                                Send
                            </Button>
                        </div>
                    </>
                )}
            </div>
        </main>
    );
}

export default App;
