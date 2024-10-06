const loadingOverlay = document.getElementById("loadingOverlay") || null;
const errorMessage = document.getElementById("error-message") || null;
let websocket = null;

// Function to initialize WebSocket connection
function connectWebSocket() {
  websocket = new WebSocket("ws://cuybot.local:81");

  websocket.onopen = function () {
    if (loadingOverlay) {
      loadingOverlay.style.display = "none";
    }
    if (errorMessage) {
      errorMessage.style.display = "none";
    }
    console.log("WebSocket connected");
  };

  websocket.onerror = function (error) {
    if (errorMessage) {
      errorMessage.style.display = "block";
    }
    console.error("WebSocket error:", error);
  };

  websocket.onclose = function () {
    if (errorMessage) {
      errorMessage.style.display = "block";
    }
    console.log("WebSocket connection closed, retrying...");
    setTimeout(connectWebSocket, 5000);
  };
}

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

// Close the WebSocket when the page is unloaded
window.addEventListener("beforeunload", function () {
  if (websocket.readyState === WebSocket.OPEN) {
    websocket.close();
  }
});