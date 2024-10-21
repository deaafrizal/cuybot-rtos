function toggleSidebar() {
  const sidebar = document.getElementById('sidebar');

  const isOpen = sidebar.classList.contains('open');

  if (isOpen) {
    sidebar.classList.remove('open');
    showContent();
  } else {
    sidebar.classList.add('open');
    hideContent();
  }
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    // hideLoadingOverlay();
  }
}

function hideContent() {
  const bodyChildren = document.body.children;

  for (let i = 0; i < bodyChildren.length; i++) {
    const child = bodyChildren[i];
    if (child.tagName !== 'ASIDE') {
      child.style.display = 'none';
    }
  }
}

function showContent() {
  const bodyChildren = document.body.children;

  for (let i = 0; i < bodyChildren.length; i++) {
    const child = bodyChildren[i];
    if (child.tagName !== 'ASIDE') {
      child.style.display = '';
    }
  }
}
function goToPage(url) {
  window.location.href = url;
}

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

function goBack() {
  window.history.back();
}

function reloadPage() {
  window.location.reload();
}

document.addEventListener('contextmenu', function (e) {
  e.preventDefault();
}, false);

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