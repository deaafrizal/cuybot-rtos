const joystickContainer = document.getElementById('joystick-container');
const rightGaugeFill = document.getElementById('rightGaugeFill');
const leftGaugeFill = document.getElementById('leftGaugeFill');

function sendWebSocketMessage(message) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(message);
  }
}

const manager = nipplejs.create({
  zone: joystickContainer,
  mode: 'static',
  position: { left: '50%', top: '50%' },
  color: 'black',
  size: 200,
});

function updateGauge(value, fillElement) {
  const clampedValue = Math.max(-100, Math.min(value, 100));
  const percentage = Math.abs(clampedValue);

  fillElement.style.width = `${percentage}%`;

  if (clampedValue === 0) {
    fillElement.style.background = "#f7f7f7";
  } else if (clampedValue > 0) {
    fillElement.style.background = "linear-gradient(to right, #f7f7f7, #007aff)";
  } else if (clampedValue < 0) {
    fillElement.style.background = "linear-gradient(to right, #f7f7f7, #f20)";
  }
}

const maxDistance = 75;
const centerDeadzone = 25;
const lateralDeadzone = 10;

let lastLeftMotor = 0;
let lastRightMotor = 0;

function calculateMotorSpeeds(distance, angle) {
  angle = (angle + 360) % 360;

  if (distance < (centerDeadzone / 100) * maxDistance) {
    lastLeftMotor = 0;
    lastRightMotor = 0;
    return { rightMotor: 0, leftMotor: 0 };
  }

  const speed = Math.min(distance, maxDistance) / maxDistance * 100;
  let rightMotor = 0;
  let leftMotor = 0;

  const edgeDeadzone = 10;
  const horizontalDeadzone = 10;

  if (
    (angle >= 90 - horizontalDeadzone && angle <= 90 + horizontalDeadzone) ||
    (angle >= 270 - horizontalDeadzone && angle <= 270 + horizontalDeadzone)
  ) {
    if (angle >= 90 - horizontalDeadzone && angle <= 90 + horizontalDeadzone) {
      leftMotor = speed;
      rightMotor = speed;
    } else {
      leftMotor = -speed;
      rightMotor = -speed;
    }
  }
  else if (angle >= edgeDeadzone && angle <= 180 - edgeDeadzone) {
    if (angle > 90 - lateralDeadzone && angle < 90 + lateralDeadzone) {
      leftMotor = speed;
      rightMotor = speed;
    } else {
      const turnFactor = Math.abs((angle - 90) / 90);
      if (angle <= 45) {
        rightMotor = speed * (1 - turnFactor);
        leftMotor = speed;
      } else {
        leftMotor = speed * (1 - turnFactor);
        rightMotor = speed;
      }
    }
  }
  else if (angle >= 180 + edgeDeadzone && angle <= 360 - edgeDeadzone) {
    const turnFactor = Math.abs((angle - 270) / 90);

    if (angle > 225 && angle < 315) {
      leftMotor = -speed;
      rightMotor = -speed;
    } else {
      if (angle <= 225) {
        leftMotor = -speed * (1 - turnFactor);
        rightMotor = -speed;
      } else {
        leftMotor = -speed;
        rightMotor = -speed * (1 - turnFactor);
      }
    }
  }
  else if (angle < edgeDeadzone || angle > 360 - edgeDeadzone) {
    leftMotor = lastLeftMotor;
    rightMotor = lastRightMotor;
  } else if (angle > 180 - edgeDeadzone && angle < 180 + edgeDeadzone) {
    leftMotor = lastLeftMotor;
    rightMotor = lastRightMotor;
  }

  lastLeftMotor = leftMotor;
  lastRightMotor = rightMotor;

  rightMotor = Math.max(-100, Math.min(100, Math.round(rightMotor)));
  leftMotor = Math.max(-100, Math.min(100, Math.round(leftMotor)));

  return { rightMotor, leftMotor };
}

manager.on('move', (evt, data) => {
  if (data) {
    const distance = data.distance || 0;
    const angle = data.angle.degree || 0;

    const { rightMotor, leftMotor } = calculateMotorSpeeds(distance, angle);

    const message = `S${rightMotor},${leftMotor}`;
    sendWebSocketMessage(message);

    updateGauge(rightMotor, rightGaugeFill);
    updateGauge(leftMotor, leftGaugeFill);
  }
});

manager.on('end', () => {
  const message = `S0,0`;
  sendWebSocketMessage(message);
  updateGauge(0, rightGaugeFill);
  updateGauge(0, leftGaugeFill);
});