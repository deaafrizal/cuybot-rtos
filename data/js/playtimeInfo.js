function updatePlaytimeDisplay(playtimeSeconds) {
  const hours = Math.floor(playtimeSeconds / 3600);
  const minutes = Math.floor((playtimeSeconds % 3600) / 60);
  const seconds = playtimeSeconds % 60;

  const playtimeElement = document.getElementById('play-time');
  if (playtimeElement) {
    playtimeElement.innerText = `${hours}h ${minutes}m ${seconds}s`;
  }
}
window.updatePlaytimeDisplay = updatePlaytimeDisplay;