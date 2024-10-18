const loadingOverlay = document.getElementById("loadingOverlay") || null;
const errorMessage = document.getElementById("error-message") || null;
let websocket = null;
let reconnecting = false; // Reconnection flag to prevent multiple attempts
let isConnected = false; // Track if the WebSocket is connected

// Function to show the loading overlay
function showLoadingOverlay(message = "Loading, please wait...") {
  if (loadingOverlay && !isConnected) { // Only show overlay if not connected
    loadingOverlay.style.display = "flex";
    loadingOverlay.querySelector("p").innerText = message; // Update the message
  }
}

// Function to hide the loading overlay
function hideLoadingOverlay() {
  if (loadingOverlay) {
    loadingOverlay.style.display = "none";
  }
}

// Function to show error message
function showError(message = "Connection error, retrying...") {
  if (errorMessage) {
    errorMessage.style.display = "block";
    errorMessage.innerText = message;
  }
}

// Function to hide error message
function hideError() {
  if (errorMessage) {
    errorMessage.style.display = "none";
  }
}

// Function to initialize WebSocket connection
function connectWebSocket() {
  if (!reconnecting) {
    showLoadingOverlay("Connecting to Cuybot...");
  }

  websocket = new WebSocket("ws://cuybot.local:81");

  websocket.onopen = function () {
    console.log("WebSocket connected");
    hideLoadingOverlay();
    hideError();
    reconnecting = false; // Reset reconnecting flag
    isConnected = true; // WebSocket is now connected
  };

  websocket.onerror = function (error) {
    console.error("WebSocket error:", error);
    showError("WebSocket error, retrying...");
    isConnected = false; // Set connected status to false on error
  };

  websocket.onclose = function () {
    console.log("WebSocket connection closed, retrying...");
    showError("WebSocket connection lost. Reconnecting...");
    reconnecting = true;
    isConnected = false; // WebSocket is no longer connected
    setTimeout(connectWebSocket, 5000); // Retry connection after 5 seconds
  };

  websocket.onmessage = function (event) {
    const data = JSON.parse(event.data);

    // Update battery voltage and percentage
    const batteryVoltage = data.batteryVoltage.toFixed(2);
    const batteryPercentage = data.batteryPercentage.toFixed(2);

    document.getElementById('battery-voltage').innerText = "⚡ " + batteryVoltage + "v";
    document.getElementById('battery-percentage').innerText = "🔋 " + batteryPercentage + "%";
  };
}

// Function to handle sidebar open/close logic (just a placeholder, modify according to your actual logic)
function toggleSidebar() {
  const sidebar = document.getElementById("sidebar");
  if (sidebar) {
    sidebar.classList.toggle("open");
  }

  // Ensure that loading overlay is not triggered just by opening the sidebar
  if (isConnected) {
    hideLoadingOverlay();
  }
}

// Initial WebSocket connection
connectWebSocket();

const speedJoystick = document.getElementById('speedJoystick');
const directionJoystick = document.getElementById('directionJoystick');

let currentSpeed = 0; // Current speed for the car
let currentDirection = 0; // Current direction
const maxSpeed = 100; // Maximum speed value

// Function to send WebSocket message
function sendMessage() {
  sendWebSocketMessage(`S${currentSpeed}D${currentDirection}`);
}

// Add joystick event listeners for speed joystick
speedJoystick.addEventListener('joystickmove', (event) => {
  const force = parseFloat(event.target.dataset.force); // Get the force value from dataset
  const direction = event.target.dataset.direction; // Get the direction from dataset

  // Update currentSpeed based on joystick position
  if (force > 0) {
    if (direction === 'n') { // Moving up
      currentSpeed = Math.min(Math.floor((force / 1) * maxSpeed), maxSpeed); // Forward
    } else if (direction === 's') { // Moving down
      currentSpeed = -Math.min(Math.floor((force / 1) * maxSpeed), maxSpeed); // Backward
    }
  } else {
    currentSpeed = 0; // Stop if no force
  }

  // Send the message
  sendMessage();
});

// Add joystick event listeners for direction joystick
directionJoystick.addEventListener('joystickmove', (event) => {
  const force = parseFloat(event.target.dataset.force); // Get the force value from dataset
  const direction = event.target.dataset.direction; // Get the direction from dataset

  // Update currentDirection based on joystick position
  if (force > 0) {
    if (direction === 'n') {
      currentDirection = 0; // Center (no turning)
    } else if (direction === 'e') {
      currentDirection = 1; // Turning right
    } else if (direction === 'w') {
      currentDirection = -1; // Turning left
    }
  } else {
    currentDirection = 0; // Center (no turning) if no force
  }

  // Send the message
  sendMessage();
});

// Handle joystick release
speedJoystick.addEventListener('joystickup', () => {
  currentSpeed = 0; // Reset speed on release
  sendMessage(); // Send stop command
});

// Handle joystick release
directionJoystick.addEventListener('joystickup', () => {
  currentDirection = 0; // Reset direction on release
  sendMessage(); // Send stop command
});

// Send motor commands via WebSocket
function sendWebSocketMessage(message) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(message);
    console.log("Sent message:", message);
  } else {
    console.error("WebSocket is not open. Cannot send message:", message);
  }
}