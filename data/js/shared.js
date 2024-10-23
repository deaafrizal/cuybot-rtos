let websocket = null;
let reconnecting = false;

function connectWebSocket() {
  websocket = new WebSocket("ws://cuybot.local:81");

  websocket.onopen = function () {
    reconnecting = false;
    console.log("CuyBot Connected!");
    handleWebSocketConnection();
  };

  websocket.onclose = function () {
    console.log("CuyBot Disconnected!");
    reconnecting = true;
    handleWebSocketDisconnection();
    setTimeout(connectWebSocket, 5000);
  };

  websocket.onerror = function (error) {
    console.log("please wait...");
    reconnecting = true;
    handleWebSocketDisconnection();
    setTimeout(connectWebSocket, 5000);
  };

  websocket.onmessage = function (event) {
    try {
      const data = JSON.parse(event.data);
          if (data.clientID !== undefined) {
        clientId = data.clientID;
      }

      if (data.playtime !== undefined) {
        updatePlaytimeDisplay(data.playtime);
      }

      if (data.usedStackPercentage !== undefined && data.taskName !== undefined) {
        updateStackInfo(data.taskName, data.usedStackPercentage);
      }

      if (data.mode !== undefined) {
        updateModeButtons(data.mode);
      }

      if (data.freeMemPercentage !== undefined && data.connectionStatus !== undefined) {
        updateHardwareInfo(data.freeMemPercentage, data.connectionStatus);
      }

      if (data.batteryVoltage !== undefined && data.batteryPercentage !== undefined) {
        updateBatteryDisplay(data.batteryVoltage, data.batteryPercentage);
      }
    } catch (e) {
      console.error("Error parsing WebSocket message:", e);
    }
  };
}

function handleWebSocketConnection() {
  startPlaytimeTracking();
}

function handleWebSocketDisconnection() {
  stopPlaytimeTracking();
}

window.addEventListener('beforeunload', () => {
  if (websocket) {
    websocket.close();
  }
});

connectWebSocket();