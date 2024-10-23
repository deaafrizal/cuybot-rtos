function updateHardwareInfo(freemem) {
  const freeMemoryElement = document.getElementById('free-memory');

  if (freeMemoryElement) {
    freeMemoryElement.innerText = `${freemem}%`;
  }
}

window.updateBatteryDisplay = updateBatteryDisplay;