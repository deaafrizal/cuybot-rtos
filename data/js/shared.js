let websocket = null;  // Global WebSocket variable
let reconnecting = false; // Track if the WebSocket is reconnecting

// Connect to WebSocket
function connectWebSocket() {
  websocket = new WebSocket("ws://cuybot.local:81"); // Change to your WebSocket address

  websocket.onopen = function () {
    console.log("WebSocket connected");
    reconnecting = false;
  };

  websocket.onclose = function () {
    console.log("WebSocket connection closed, retrying...");
    reconnecting = true;
    setTimeout(connectWebSocket, 5000); // Retry connection after 5 seconds
  };

  websocket.onerror = function (error) {
    console.error("WebSocket error:", error);
    reconnecting = true;
    setTimeout(connectWebSocket, 5000); // Retry on error
  };

  websocket.onmessage = function (event) {
    try {
      const data = JSON.parse(event.data);

      // Handle the mode update
      if (data.mode !== undefined) {
        updateModeButtons(data.mode); // Call from mode.js to update button states
      }

      // If mode.js is waiting for WebSocket response, reset button states
      if (typeof resetButton === "function" && activeButton) {
        resetButton(activeButton);
      }
    } catch (e) {
      console.error("Error parsing WebSocket message:", e);
    }
  };
}

// Disconnect WebSocket on page unload
window.addEventListener('beforeunload', () => {
  if (websocket) {
    websocket.close();
  }
});

// Initialize WebSocket connection
connectWebSocket();