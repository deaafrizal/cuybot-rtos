let activeButton = null; // Track the currently active button
let responseTimeout = null;

// Get the mode buttons
const obstacleButton = document.getElementById('obstacleButton');
const followButton = document.getElementById('followButton');
const patrolButton = document.getElementById('patrolButton');
const spinButton = document.getElementById('spinButton');

// Button click event listeners
obstacleButton.addEventListener('click', () => handleButtonClick(obstacleButton, 'M2'));
followButton.addEventListener('click', () => handleButtonClick(followButton, 'M3'));
patrolButton.addEventListener('click', () => handleButtonClick(patrolButton, 'M4'));
spinButton.addEventListener('click', () => handleButtonClick(spinButton, 'M5'));

// Handle button clicks
function handleButtonClick(button, modeMessage) {
  if (button.classList.contains('loading')) return; // Prevent clicking when a button is loading

  if (button === activeButton) {
    // If the clicked button is the active one, deactivate it
    sendModeMessage('M1'); // Send message for manual mode (default)
    setButtonLoading(button, 'Deactivating...');
  } else {
    // Otherwise, activate the clicked button
    sendModeMessage(modeMessage);
    setButtonLoading(button, 'Activating...');
    activeButton = button; // Set the clicked button as active
  }
}

// Set button as loading
function setButtonLoading(button, loadingText) {
  button.classList.add('loading');
  button.dataset.originalText = button.innerText; // Store original text
  button.innerText = loadingText;

  // Disable other buttons while loading
  disableOtherButtons(true);

  // Optional: add a timeout in case the WebSocket takes too long
  clearTimeout(responseTimeout);
  responseTimeout = setTimeout(() => {
    handleWebSocketTimeout(button);
  }, 1500);
}

// Reset button after loading or deactivation
function resetButton(button) {
  button.classList.remove('loading'); // Remove 'loading' class
  button.disabled = false; // Re-enable the button
  button.innerText = button.dataset.originalText || button.innerText; // Reset text

  // Reset the button's background color and text color when deactivated
  button.style.backgroundColor = ''; // Clear background color
  button.style.color = ''; // Clear text color

  // Enable other buttons once the current one is reset
  disableOtherButtons(false);
}

// Disable or enable other buttons
function disableOtherButtons(disable) {
  const buttons = [obstacleButton, followButton, patrolButton, spinButton];
  buttons.forEach(btn => {
    if (btn !== activeButton) {
      btn.disabled = disable;
    }
  });
}

// Handle WebSocket timeout
function handleWebSocketTimeout(button) {
  resetButton(button);
}

const savedMode = sessionStorage.getItem('activeMode');

if (savedMode) {
  updateModeButtons(savedMode);
  sendModeMessage(`M${savedMode}`);
}

// Update button styles based on active mode from WebSocket
function updateModeButtons(activeMode) {
  const buttons = [obstacleButton, followButton, patrolButton, spinButton];

  sessionStorage.setItem('activeMode', activeMode);

  // If manual mode is activated, deactivate all buttons
  if (activeMode === 'M1' || activeMode === '1') {
    if (activeButton) {
      resetButton(activeButton); // Reset the currently active button
    }
    activeButton = null; // Clear the activeButton reference
  } else {
    buttons.forEach(button => {
      if (button.dataset.mode === `M${activeMode}`) { // Compare activeMode as 'M2', 'M3', etc.
        // Activate the button corresponding to the active mode
        activeButton = button;
        button.classList.add('active');
        button.style.backgroundColor = "#007aff"; // Set background color to blue when active
        button.style.color = "#fff"; // Set text color to white when active
      } else {
        // Reset any other buttons that are not active
        button.style.backgroundColor = ''; // Reset background color
        button.style.color = ''; // Reset text color to default
        button.classList.remove('active');
      }
    });
  }
}

// Example WebSocket send message function
function sendModeMessage(message) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(message);
  }
}

// When WebSocket receives a message
function onWebSocketMessage(data) {
  const { mode } = data;
  if (mode) {
    updateModeButtons(mode);
  }

  clearTimeout(responseTimeout); // Clear the timeout when response is received

  // Reset button after activation or deactivation
  if (activeButton) {
    resetButton(activeButton);
  }
}