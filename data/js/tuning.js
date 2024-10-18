// Fetch system settings for max motor speed and motor weight
async function fetchSystemSettings() {
  try {
    const response = await fetch('http://cuybot.local/api/getSystemData');
    const data = await response.json();

    // Correctly reference the API response keys
    if (data.hasOwnProperty('motorMaxSpeed') && data.hasOwnProperty('motorWeight')) {
      // Convert PWM (0-255) to percentage for motorMaxSpeed
      const maxSpeedPercentage = Math.round((data.motorMaxSpeed / 255) * 100);

      // Convert PWM (0-255) to percentage for motorWeight
      const weightPercentage = Math.round((data.motorWeight / 255) * 100);

      // Set max motor speed slider (as percentage)
      const speedSlider = document.getElementById('speedSlider');
      speedSlider.value = maxSpeedPercentage;  // Dynamically set the value
      document.getElementById('speedLabel').textContent = `${maxSpeedPercentage}%`;  // Display percentage

      // Set motor weight slider (as percentage)
      const weightSlider = document.getElementById('weightSlider');
      weightSlider.value = weightPercentage;  // Dynamically set the value
      document.getElementById('weightLabel').textContent = `${weightPercentage}%`;  // Display percentage
    } else {
      console.error('motorMaxSpeed or motorWeight not found in API response');
    }
  } catch (error) {
    console.error('Error fetching system settings:', error);
  }
}

// Function to update the motor speed (send PWM value)
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

// Function to update the motor weight (send PWM value)
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

// Submit button event listener to send the updated settings to the backend
document.getElementById('submitButton').addEventListener('click', async function () {
  const speedSlider = document.getElementById('speedSlider');
  const weightSlider = document.getElementById('weightSlider');

  // Convert the slider values (percentage) to PWM (0-255)
  const maxSpeedPWM = Math.round((speedSlider.value / 100) * 255);
  const motorWeightPWM = Math.round((weightSlider.value / 100) * 255);

  try {
    // Send the new motorMaxSpeed and motorWeight to the backend and handle both responses
    await Promise.all([updateMotorSpeed(maxSpeedPWM), updateMotorWeight(motorWeightPWM)]);
    // If both updates succeed, show the success notification
    showNotification('Your tuning is saved!', 'success');
  } catch (error) {
    // If any of the updates fail, show the failure notification
    console.error(error);
    showNotification('Your tuning is not saved', 'error');
  }
});

// Function to display notifications
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
  // Fetch system settings on page load
  fetchSystemSettings();

  // Add event listeners for sliders to show their values
  document.getElementById('speedSlider').addEventListener('input', function () {
    document.getElementById('speedLabel').textContent = this.value + '%';
  });

  document.getElementById('weightSlider').addEventListener('input', function () {
    document.getElementById('weightLabel').textContent = this.value + '%';
  });
});