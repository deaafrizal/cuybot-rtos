function toggleSidebar() {
  const sidebar = document.getElementById('sidebar');

  // Check if sidebar is currently open
  const isOpen = sidebar.classList.contains('open');

  if (isOpen) {
    sidebar.classList.remove('open');
    showContent();  // Show the hidden content
  } else {
    sidebar.classList.add('open');
    hideContent();  // Hide all content except the sidebar
  }
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    hideLoadingOverlay();
  }
}

// Hide all elements except the sidebar
function hideContent() {
  const bodyChildren = document.body.children;

  for (let i = 0; i < bodyChildren.length; i++) {
    const child = bodyChildren[i];
    if (child.tagName !== 'ASIDE') {
      child.style.display = 'none';  // Hide all elements except <aside>
    }
  }
}

// Show all elements that were hidden
function showContent() {
  const bodyChildren = document.body.children;

  for (let i = 0; i < bodyChildren.length; i++) {
    const child = bodyChildren[i];
    if (child.tagName !== 'ASIDE') {
      child.style.display = '';  // Reset display to show elements again
    }
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

// Disable right-click
document.addEventListener('contextmenu', function (e) {
  e.preventDefault();
}, false);

// Disable touch-and-hold (on mobile devices)
document.addEventListener('touchstart', function (e) {
  if (e.touches.length > 1) {
    e.preventDefault();
  }
}, false);

document.addEventListener('mousedown', function (e) {
  if (e.detail > 1) {
    e.preventDefault();
  }
}, false);