const speedJoystick = document.getElementById('speedJoystick');
const directionJoystick = document.getElementById('directionJoystick');
let currentSpeed = 0;
let currentDirection = 0;
const maxSpeed = 90;

function sendJoystickControlMessage() {
  const message = `S${currentSpeed}D${currentDirection}`;
  sendWebSocketMessage(message);
}

function sendWebSocketMessage(message) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(message);
  }
}

speedJoystick.addEventListener('joystickmove', (event) => {
  const force = parseFloat(event.target.dataset.force);
  const direction = event.target.dataset.direction;
  if (force > 0) {
    if (direction === 'n') {
      currentSpeed = Math.min(Math.floor((force / 1) * maxSpeed), maxSpeed);
      console.log(currentSpeed);
    } else if (direction === 's') {
      currentSpeed = -Math.min(Math.floor((force / 1) * maxSpeed), maxSpeed);
      console.log(currentSpeed);
    }
  } else {
    currentSpeed = 0;
  }

  sendJoystickControlMessage();
});

directionJoystick.addEventListener('joystickmove', (event) => {
  const force = parseFloat(event.target.dataset.force);
  const direction = event.target.dataset.direction;

  if (force > 0) {
    if (direction === 'n') {
      currentDirection = 0;
    } else if (direction === 'e') {
      currentDirection = 1;
    } else if (direction === 'w') {
      currentDirection = -1;
    }
  } else {
    currentDirection = 0;
  }

  sendJoystickControlMessage();
});

speedJoystick.addEventListener('joystickup', () => {
  currentSpeed = 0;
  sendJoystickControlMessage();
});

directionJoystick.addEventListener('joystickup', () => {
  currentDirection = 0;
  sendJoystickControlMessage();
});