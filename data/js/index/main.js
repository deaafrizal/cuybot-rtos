function toggleSidebar() {
  const sidebar = document.getElementById('sidebar');
  const joystickContainer = document.getElementById('joystickContainer');

  if (sidebar.classList.contains('open')) {
    // Close the sidebar, show joysticks
    sidebar.classList.remove('open');
    joystickContainer.style.display = 'flex'; // Show joysticks again
  } else {
    // Open the sidebar, hide joysticks
    sidebar.classList.add('open');
    joystickContainer.style.display = 'none'; // Hide the joysticks
  }
}
// Handle navigation to different pages
function goToPage(url) {
  window.location.href = url; // Navigate to the specified URL
}

// Reboot system function (if applicable)
function rebootSystem() {
  fetch('http://cuybot.local/api/reboot', {
    method: 'POST'
  })
    .then(response => {
      if (response.ok) {
        alert('System rebooting...');
      } else {
        alert('Failed to reboot system.');
      }
    })
    .catch(error => {
      console.error('Error rebooting system:', error);
    });
}

// Back button functionality
function goBack() {
  window.history.back(); // Go back to the previous page
}

function reloadPage() {
  window.location.reload();
}
