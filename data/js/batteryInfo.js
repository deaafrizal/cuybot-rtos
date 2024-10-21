function updateBatteryDisplay(voltage, percentage) {
  const batteryVoltageElement = document.getElementById('battery-voltage');
  const batteryPercentageElement = document.getElementById('battery-percentage');

  if (batteryVoltageElement && batteryPercentageElement) {
    batteryVoltageElement.innerText = `${voltage}V`;
    batteryPercentageElement.innerText = `${percentage}%`;
  }
}

window.updateBatteryDisplay = updateBatteryDisplay;