async function fetchSystemSettings() {
  try {
    const response = await fetch('http://cuybot.local/api/getSystemData');
    const data = await response.json();

    if (data.hasOwnProperty('motorMaxSpeed') && data.hasOwnProperty('motorWeight')) {
      const speedSlider = document.getElementById('speedSlider').value;
      document.getElementById('speedLabel').textContent = `${speedSlider} pwm`;

      const weightSlider = document.getElementById('weightSlider').value;
      document.getElementById('weightLabel').textContent = `${weightSlider} pwm`;
    } else {
      console.error('motorMaxSpeed or motorWeight not found in API response');
    }
  } catch (error) {
    console.error('Error fetching system settings:', error);
  }
}

async function updateMotorSpeed(pwmValue) {
  try {
    const response = await fetch('http://cuybot.local/api/setMotorMaxSpeed', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `motorMaxSpeed=${pwmValue}`
    });
    const data = await response.json();
    if (data.status !== "success") {
      throw new Error('Failed to update motor speed');
    }
  } catch (error) {
    throw new Error('Error in updateMotorSpeed: ' + error.message);
  }
}

async function updateMotorWeight(pwmValue) {
  try {
    const response = await fetch('http://cuybot.local/api/setMotorWeight', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `motorWeight=${pwmValue}`
    });
    const data = await response.json();
    if (data.status !== "success") {
      throw new Error('Failed to update motor weight');
    }
  } catch (error) {
    throw new Error('Error in updateMotorWeight: ' + error.message);
  }
}

document.getElementById('submitButton').addEventListener('click', async function () {
  const speedSlider = document.getElementById('speedSlider');
  const weightSlider = document.getElementById('weightSlider');

  const maxSpeedPWM = speedSlider.value;
  const motorWeightPWM = weightSlider.value;

  try {
    await Promise.all([updateMotorSpeed(maxSpeedPWM), updateMotorWeight(motorWeightPWM)]);
    showNotification('Your tuning is saved!', 'success');
  } catch (error) {
    console.error(error);
    showNotification('Your tuning is not saved', 'error');
  }
});

function showNotification(message, type) {
  const notification = document.getElementById('notification');
  notification.textContent = message;
  notification.className = type === 'success' ? 'success' : 'error';
  notification.style.display = 'block';
  setTimeout(() => {
    notification.style.display = 'none';
  }, 3000);
}

window.addEventListener('load', function () {
  fetchSystemSettings();

  document.getElementById('speedSlider').addEventListener('input', function () {
    document.getElementById('speedLabel').textContent = this.value + ' pwm';
  });

  document.getElementById('weightSlider').addEventListener('input', function () {
    document.getElementById('weightLabel').textContent = this.value + ' pwm';
  });
});