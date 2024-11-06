let activeButton = null;
let responseTimeout = null;

const obstacleButton = document.getElementById('obstacleButton');
const followButton = document.getElementById('followButton');
const patrolButton = document.getElementById('patrolButton');
const spinButton = document.getElementById('spinButton');

obstacleButton.addEventListener('click', () => handleButtonClick(obstacleButton, 'M2'));
followButton.addEventListener('click', () => handleButtonClick(followButton, 'M3'));
patrolButton.addEventListener('click', () => handleButtonClick(patrolButton, 'M4'));
spinButton.addEventListener('click', () => handleButtonClick(spinButton, 'M5'));

function handleButtonClick(button, modeMessage) {
  if (button.classList.contains('loading')) return;

  if (button === activeButton) {
    sendModeMessage('M1');
    setButtonLoading(button, 'Deactivating...');
    activeButton = null;
  } else {
    sendModeMessage(modeMessage);
    setButtonLoading(button, 'Activating...');
    if (activeButton) {
      resetButton(activeButton);
    }
    activeButton = button;
  }
}

function setButtonLoading(button, loadingText) {
  button.classList.add('loading');
  button.dataset.originalText = button.innerText;
  button.innerText = loadingText;

  disableOtherButtons(true);

  clearTimeout(responseTimeout);
  responseTimeout = setTimeout(() => {
    handleWebSocketTimeout(button);
  }, 1500);
}

function resetButton(button) {
  button.classList.remove('loading');
  button.disabled = false;
  button.innerText = button.dataset.originalText || button.innerText;

  button.style.backgroundColor = '';
  button.style.color = '';

  disableOtherButtons(false);
}

function disableOtherButtons(disable) {
  const buttons = [obstacleButton, followButton, patrolButton, spinButton];
  buttons.forEach(btn => {
    if (btn !== activeButton) {
      btn.disabled = disable;
    }
  });
}

function handleWebSocketTimeout(button) {
  resetButton(button);
}

function updateModeButtons(activeMode) {
  const buttons = [obstacleButton, followButton, patrolButton, spinButton];

  sessionStorage.setItem('activeMode', activeMode);

  buttons.forEach(button => {
    if (button.dataset.mode === `M${activeMode}`) {
      activeButton = button;
      button.classList.add('active');
      button.style.backgroundColor = "#007aff";
      button.style.color = "#fff";
    } else {
      button.style.backgroundColor = '';
      button.style.color = '';
      button.classList.remove('active');
    }
  });
}

const savedMode = sessionStorage.getItem('activeMode');

if (savedMode) {
  updateModeButtons(savedMode);
  sendModeMessage(`M${savedMode}`);
}


function sendModeMessage(message) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(message);
  }
}

function onWebSocketMessage(data) {
  const { mode } = data;
  if (mode) {
    updateModeButtons(mode);
  }

  clearTimeout(responseTimeout);

  if (activeButton) {
    resetButton(activeButton);
  }
}