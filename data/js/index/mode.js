document.addEventListener("DOMContentLoaded", function () {
  const obstacleSwitch = document.getElementById("obstacleSwitch");
  const followSwitch = document.getElementById("followSwitch");
  const errorMessage = document.getElementById("error-message");

  const MODES = {
    MANUAL: 1,
    OBSTACLE: 2,
    FOLLOW: 3
  };

  initSwitches();

  function initSwitches() {
    obstacleSwitch.addEventListener("change", function () {
      handleSwitchChange(obstacleSwitch, followSwitch, MODES.OBSTACLE);
    });

    followSwitch.addEventListener("change", function () {
      handleSwitchChange(followSwitch, obstacleSwitch, MODES.FOLLOW);
    });
  }

  function handleSwitchChange(activeSwitch, inactiveSwitch, mode) {
    // Temporarily disable both switches until the response is received
    obstacleSwitch.disabled = true;
    followSwitch.disabled = true;

    const isActive = activeSwitch.checked; // Save current state of the active switch
    const selectedMode = isActive ? mode : MODES.MANUAL;

    updateMode(selectedMode)
      .then(success => {
        if (success) {
          // Server responded with success, so update the switch states
          inactiveSwitch.checked = false;
          activeSwitch.checked = isActive;
        } else {
          // If not successful, revert the switch state
          activeSwitch.checked = !isActive;
        }
        obstacleSwitch.disabled = false;
        followSwitch.disabled = false;
      })
      .catch(error => {
        // Handle any errors and revert switch state
        console.error("Error:", error);
        activeSwitch.checked = !isActive; // Revert switch state if error
        obstacleSwitch.disabled = false;
        followSwitch.disabled = false;
      });
  }

  function updateMode(newMode) {
    return fetch("http://cuybot.local/api/setMode", {
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
      },
      body: `mode=${newMode}`,
    })
      .then(response => {
        if (!response.ok) {
          throw new Error("Failed to set mode");
        }
        return response.json();
      })
      .then(data => {
        console.log("Mode set successfully:", data);
        errorMessage.style.display = "none";
        return true; // Indicate success
      })
      .catch(error => {
        console.error("Error setting mode:", error);
        errorMessage.style.display = "block";
        return false; // Indicate failure
      });
  }
});