function toggleSidebar() {
  const sidebar = document.getElementById('sidebar');

  // Toggle the open class to show/hide the sidebar
  sidebar.classList.toggle('open');

  // Add or remove the closed class based on the sidebar state
  if (sidebar.classList.contains('open')) {
    sidebar.classList.remove('closed'); // Remove closed class when opening
  } else {
    sidebar.classList.add('closed'); // Add closed class when closing
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
