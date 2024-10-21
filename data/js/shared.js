let websocket = null;
let reconnecting = false;

function connectWebSocket() {
  websocket = new WebSocket("ws://cuybot.local:81");

  websocket.onopen = function () {
    reconnecting = false;
    console.log("WebSocket connected");
    handleWebSocketConnection();
  };

  websocket.onclose = function () {
    console.log("WebSocket connection closed, retrying...");
    reconnecting = true;
    handleWebSocketDisconnection();
    setTimeout(connectWebSocket, 5000);
  };

  websocket.onerror = function (error) {
    console.error("WebSocket error:", error);
    reconnecting = true;
    handleWebSocketDisconnection();
    setTimeout(connectWebSocket, 5000);
  };

  websocket.onmessage = function (event) {
    try {
      const data = JSON.parse(event.data);
      console.log(data.freeStackPercentage);
      if (data.clientID !== undefined) {
        clientId = data.clientID;
      }

      if (data.playtime !== undefined) {
        updatePlaytimeDisplay(data.playtime);
      }

      if (data.freeStackPercentage !== undefined && data.usedStackPercentage !== undefined) {
        updateStackInfo(data.freeStackPercentage, data.usedStackPercentage);
      }

      if (data.mode !== undefined) {
        updateModeButtons(data.mode);
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