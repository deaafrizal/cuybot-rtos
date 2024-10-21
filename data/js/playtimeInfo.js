// playtime.js

// Function to update the playtime display in the UI
function updatePlaytimeDisplay(playtimeSeconds) {
  const hours = Math.floor(playtimeSeconds / 3600);
  const minutes = Math.floor((playtimeSeconds % 3600) / 60);
  const seconds = playtimeSeconds % 60;

  const playtimeElement = document.getElementById('play-time');
  if (playtimeElement) {
    playtimeElement.innerText = `${hours}h ${minutes}m ${seconds}s`;
  }
}

// Playtime tracking functions
let playtimeInterval = null;

function startPlaytimeTracking() {
  if (!playtimeInterval) {
    playtimeInterval = setInterval(() => {
      // console.log("Tracking playtime..."); // Placeholder, playtime is managed by the backend
    }, 1000);
  }
}

function stopPlaytimeTracking() {
  clearInterval(playtimeInterval);
  playtimeInterval = null;
}

// Expose functions globally
window.updatePlaytimeDisplay = updatePlaytimeDisplay;
window.startPlaytimeTracking = startPlaytimeTracking;
window.stopPlaytimeTracking = stopPlaytimeTracking;