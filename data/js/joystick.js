const rightMotorJoystick = document.getElementById('rightMotor');
const leftMotorJoystick = document.getElementById('leftMotor');
let rightMotorSpeed = 0;
let leftMotorSpeed = 0;
const maxSpeed = 90;

function sendMotorControlMessage() {
  const message = `R${rightMotorSpeed}L${leftMotorSpeed}`;
  sendWebSocketMessage(message);
}

function sendWebSocketMessage(message) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(message);
  }
}

rightMotorJoystick.addEventListener('joystickmove', (event) => {
  const force = parseFloat(event.target.dataset.force);
  const direction = event.target.dataset.direction;

  if (force > 0) {
    if (direction === 'n') {
      rightMotorSpeed = Math.min(Math.floor((force / 1) * maxSpeed), maxSpeed);
    } else if (direction === 's') {
      rightMotorSpeed = -Math.min(Math.floor((force / 1) * maxSpeed), maxSpeed);
    }
  } else {
    rightMotorSpeed = 0;
  }

  sendMotorControlMessage();
});

leftMotorJoystick.addEventListener('joystickmove', (event) => {
  const force = parseFloat(event.target.dataset.force);
  const direction = event.target.dataset.direction;
  if (force > 0) {
    if (direction === 'n') {
      leftMotorSpeed = Math.min(Math.floor((force / 1) * maxSpeed), maxSpeed);
    } else if (direction === 's') {
      leftMotorSpeed = -Math.min(Math.floor((force / 1) * maxSpeed), maxSpeed);
    }
  } else {
    leftMotorSpeed = 0;
  }

  sendMotorControlMessage();
});

rightMotorJoystick.addEventListener('joystickup', () => {
  rightMotorSpeed = 0;
  sendMotorControlMessage();
});

leftMotorJoystick.addEventListener('joystickup', () => {
  leftMotorSpeed = 0;
  sendMotorControlMessage();
});