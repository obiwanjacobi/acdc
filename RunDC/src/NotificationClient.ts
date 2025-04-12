import * as signalR from "@microsoft/signalr";

class NotificationClient {
    private hubConnection: signalR.HubConnection | null = null;

    public async startConnection(hubUrl: string): Promise<void> {
        if (this.hubConnection) {
            return;
        }

        this.hubConnection = new signalR.HubConnectionBuilder()
            .withUrl(hubUrl)
            .withAutomaticReconnect()
            .configureLogging(signalR.LogLevel.Information)
            .build();

        try {
            await this.hubConnection.start();
            await this.hubConnection.send("subscribe");
            console.log("SignalR connection established");
        } catch (error) {
            console.error("Error establishing SignalR connection:", error);
        }
    }

    public async stopConnection(): Promise<void> {
        if (this.hubConnection) {
            await this.hubConnection.stop();
            this.hubConnection = null;
            console.log("SignalR connection closed");
        }
    }

    public onTurnoutChanged(callback: (notification: any) => void): boolean {
        if (!this.hubConnection) {
            console.error("Cannot register event: connection not established");
            return false;
        }

        this.hubConnection.on("OnTurnoutChanged", (data) => {
            callback(data);
        });
        return true;
    }

    public onSignalChanged(callback: (notification: any) => void): boolean {
        if (!this.hubConnection) {
            console.error("Cannot register event: connection not established");
            return false;
        }

        this.hubConnection.on("OnSignalChanged", (data) => {
            callback(data);
        });
        return true;
    }

    public onBlockOccupationChanged(callback: (notification: any) => void): boolean {
        if (!this.hubConnection) {
            console.error("Cannot register event: connection not established");
            return false;
        }

        this.hubConnection.on("OnBlockOccupationChanged", (data) => {
            callback(data);
        });
        return true;
    }

    public onMessage(callback: (notification: any) => void): boolean {
        if (!this.hubConnection) {
            console.error("Cannot register event: connection not established");
            return false;
        }

        this.hubConnection.on("OnMessage", (data) => {
            callback(data);
        });
        return true;
    }
}

// Export as singleton
export const notificationClient = new NotificationClient();
