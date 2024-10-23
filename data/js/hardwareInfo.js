function updateHardwareInfo(freeMem, connectionStatus) {
  const freeMemoryElement = document.getElementById('free-memory-percentage');
  const connectionStatusElement = document.getElementById('server-connection');

  if (freeMemoryElement && connectionStatusElement) {
    freeMemoryElement.innerText = `${freeMem}%`;
    connectionStatusElement.innerText = `${connectionStatus}`;
  }
}

window.updateBatteryDisplay = updateBatteryDisplay;