import WebSerialTest from "./WebSerialTest";
import Theme from "./Theme";
import ServerNotificationTest from "./ServerNotificationTest";

function App() {
    return (
        <Theme>
            <ServerNotificationTest notificationUrl="http://localhost:5246/notifications" />
            {/* <WebSerialTest /> */}
        </Theme>
    );
}

export default App;
