const loadingOverlay = document.getElementById("loadingOverlay") || null;
const errorMessage = document.getElementById("error-message") || null;
const obstacleSwitch = document.getElementById("obstacleSwitch");
const followSwitch = document.getElementById("followSwitch");
const patrolSwitch = document.getElementById("patrolSwitch");
const speedJoystick = document.getElementById('speedJoystick');
const directionJoystick = document.getElementById('directionJoystick');

const MODES = {
  MANUAL: 1,
  OBSTACLE: 2,
  FOLLOW: 3,
  PATROL: 4,
};

let websocket = null;
let reconnecting = false;
let isConnected = false;

let currentSpeed = 0;
let currentDirection = 0;
const maxSpeed = 100;

function showLoadingOverlay(message = "Loading, please wait...") {
  if (loadingOverlay && !isConnected) { // Only show overlay if not connected
    loadingOverlay.style.display = "flex";
    loadingOverlay.querySelector("p").innerText = message; // Update the message
  }
}

function hideLoadingOverlay() {
  if (loadingOverlay) {
    loadingOverlay.style.display = "none";
  }
}

function showError(message = "Connection error, retrying...") {
  if (errorMessage) {
    errorMessage.style.display = "block";
    errorMessage.innerText = message;
  }
}

function hideError() {
  if (errorMessage) {
    errorMessage.style.display = "none";
  }
}

function toggleSidebar() {
  const sidebar = document.getElementById("sidebar");
  if (sidebar) {
    sidebar.classList.toggle("open");
  }

  if (isConnected) {
    hideLoadingOverlay();
  }
}

function connectWebSocket() {
  if (!reconnecting) {
    showLoadingOverlay("Connecting to Cuybot...");
  }

  websocket = new WebSocket("ws://cuybot.local:81");

  websocket.onopen = function () {
    websocket.send("GET_CURRENT_MODE");
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
    try {
      const data = JSON.parse(event.data);

      // Handle mode updates
      if (data?.mode !== undefined) {
        updateSwitches(data.mode);
      }

      // Update battery voltage and percentage
      if (data?.batteryVoltage && data?.batteryPercentage) {
        const batteryVoltage = data.batteryVoltage.toFixed(2);
        const batteryPercentage = data.batteryPercentage.toFixed(2);
        document.getElementById('battery-voltage').innerText = "⚡ " + batteryVoltage + "v";
        document.getElementById('battery-percentage').innerText = "🔋 " + batteryPercentage + "%";
      }

      // Handle motor control feedback from server (if necessary)
      if (data?.speed !== undefined && data?.direction !== undefined) {
        currentSpeed = data.speed;
        currentDirection = data.direction;
      }

    } catch (e) {
      console.error("Error parsing WebSocket message:", e);
    }
  };
}

connectWebSocket();
initSwitches();

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

// Handle joystick release for speed
speedJoystick.addEventListener('joystickup', () => {
  currentSpeed = 0; // Reset speed on release
  sendMessage(); // Send stop command
});

// Handle joystick release for direction
directionJoystick.addEventListener('joystickup', () => {
  currentDirection = 0; // Reset direction on release
  sendMessage(); // Send stop command
});

// Send motor commands via WebSocket
function sendWebSocketMessage(message) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(message);
  } else {
    console.error("WebSocket is not open. Cannot send message:", message);
  }
}

// Combine speed and direction into a single message and send
function sendMessage() {
  const message = `S${currentSpeed}D${currentDirection}`;
  sendWebSocketMessage(message);
}

function updateSwitches(mode) {
  obstacleSwitch.checked = (mode === MODES.OBSTACLE);
  followSwitch.checked = (mode === MODES.FOLLOW);
  patrolSwitch.checked = (mode === MODES.PATROL);
}

function debounce(func, delay) {
  let timeout;
  return function (...args) {
    clearTimeout(timeout);
    timeout = setTimeout(() => func.apply(this, args), delay);
  };
}

const debouncedHandleSwitchChange = debounce(handleSwitchChange, 300);

function initSwitches() {
  obstacleSwitch.addEventListener("change", function () {
    debouncedHandleSwitchChange(obstacleSwitch, [followSwitch, patrolSwitch], MODES.OBSTACLE);
  });

  followSwitch.addEventListener("change", function () {
    debouncedHandleSwitchChange(followSwitch, [obstacleSwitch, patrolSwitch], MODES.FOLLOW);
  });

  patrolSwitch.addEventListener("change", function () {
    debouncedHandleSwitchChange(patrolSwitch, [obstacleSwitch, followSwitch], MODES.PATROL);
  });
}

function handleSwitchChange(activeSwitch, inactiveSwitches, mode) {
  const isActive = activeSwitch.checked;
  const selectedMode = isActive ? mode : MODES.MANUAL;

  if (isActive) {
    inactiveSwitches.forEach(switchElement => {
      switchElement.checked = false; // Uncheck other switches
      switchElement.disabled = false; // Re-enable other switches
    });
  }

  const message = `M${selectedMode}`;
  sendWebSocketMessage(message);
}