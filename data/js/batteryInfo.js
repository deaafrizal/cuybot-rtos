function updateBatteryDisplay(voltage, percentage) {
  const batteryVoltageElement = document.getElementById('battery-voltage');
  const batteryPercentageElement = document.getElementById('battery-percentage');
  const powerHealth = document.getElementById('power-health');

  if (batteryVoltageElement && batteryPercentageElement) {
    batteryVoltageElement.innerText = `${voltage}V`;
    batteryPercentageElement.innerText = `${percentage}%`;
    powerHealth.innerText = `${powerHealth}%`;
  }
}

window.updateBatteryDisplay = updateBatteryDisplay;