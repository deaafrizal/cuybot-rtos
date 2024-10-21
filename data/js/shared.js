// shared.js

let websocket = null;
let reconnecting = false;

function connectWebSocket() {
  websocket = new WebSocket("ws://cuybot.local:81");

  websocket.onopen = function () {
    reconnecting = false;
    console.log("WebSocket connected");
    handleWebSocketConnection(); // Handle playtime start/resume
  };

  websocket.onclose = function () {
    console.log("WebSocket connection closed, retrying...");
    reconnecting = true;
    handleWebSocketDisconnection(); // Handle playtime pause/stop
    setTimeout(connectWebSocket, 5000); // Retry connection after 5 seconds
  };

  websocket.onerror = function (error) {
    console.error("WebSocket error:", error);
    reconnecting = true;
    handleWebSocketDisconnection(); // Handle playtime pause/stop
    setTimeout(connectWebSocket, 5000); // Retry on error
  };

  websocket.onmessage = function (event) {
    try {
      const data = JSON.parse(event.data);
      console.log("Received data:", data);
      // Check if the message contains the clientID
      if (data.clientID !== undefined) {
        clientId = data.clientID;  // Store the client ID
        console.log("Client ID received:", clientId);
      }

      // Handle playtime data
      if (data.playtime !== undefined) {
        console.log(`Playtime data received: ${data.playtime} seconds`);
        updatePlaytimeDisplay(data.playtime); // Update playtime display on UI
      }

      // Handle mode or other data
      if (data.mode !== undefined) {
        updateModeButtons(data.mode); // Update the mode buttons
      }

      // Handle battery data
      if (data.batteryVoltage !== undefined && data.batteryPercentage !== undefined) {
        updateBatteryDisplay(data.batteryVoltage, data.batteryPercentage);
      }
    } catch (e) {
      console.error("Error parsing WebSocket message:", e);
    }
  };
}

// WebSocket handling for playtime
function handleWebSocketConnection() {
  startPlaytimeTracking(); // Start playtime tracking when WebSocket is connected
}

function handleWebSocketDisconnection() {
  stopPlaytimeTracking(); // Pause playtime tracking when WebSocket is disconnected
}

// WebSocket connection management
window.addEventListener('beforeunload', () => {
  if (websocket) {
    websocket.close();
  }
});

// Connect WebSocket on page load
connectWebSocket();