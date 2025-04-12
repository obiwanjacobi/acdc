import { useEffect, useState } from "react";
import { notificationClient } from "./NotificationClient";

export interface ServerNotificationTestProps {
    notificationUrl: string;
}

const ServerNotificationTest = (props: ServerNotificationTestProps) => {
    const { notificationUrl } = props;
    const [output, setOutput] = useState<string>("");

    useEffect(() => {
        const init = async () => {
            await notificationClient.startConnection(notificationUrl);
            notificationClient.onBlockOccupationChanged((data) => setOutput((prev) => prev + "Block: " + data + "\n"));
            notificationClient.onSignalChanged((data) => setOutput((prev) => prev + "Signal: " + data + "\n"));
            notificationClient.onTurnoutChanged((data) => setOutput((prev) => prev + "Turnout: " + data + "\n"));
            notificationClient.onMessage((data) => setOutput((prev) => prev + data + "\n"));
        };

        init();

        //return () => notificationClient.stopConnection();
    }, []);

    return <pre>{output}</pre>;
};

export default ServerNotificationTest;
