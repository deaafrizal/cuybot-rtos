document.addEventListener("DOMContentLoaded", function () {
  const obstacleSwitch = document.getElementById("obstacleSwitch");
  const followSwitch = document.getElementById("followSwitch");
  const patrolSwitch = document.getElementById("patrolSwitch");
  const errorMessage = document.getElementById("error-message");

  const MODES = {
    MANUAL: 1,
    OBSTACLE: 2,
    FOLLOW: 3,
    PATROL: 4,
  };

  initSwitches();

  function initSwitches() {
    // Event listener for obstacleSwitch
    obstacleSwitch.addEventListener("change", function () {
      handleSwitchChange(obstacleSwitch, [followSwitch, patrolSwitch], MODES.OBSTACLE);
    });

    // Event listener for followSwitch
    followSwitch.addEventListener("change", function () {
      handleSwitchChange(followSwitch, [obstacleSwitch, patrolSwitch], MODES.FOLLOW);
    });

    // Event listener for patrolSwitch
    patrolSwitch.addEventListener("change", function () {
      handleSwitchChange(patrolSwitch, [obstacleSwitch, followSwitch], MODES.PATROL);
    });
  }

  function handleSwitchChange(activeSwitch, inactiveSwitches, mode) {
  // Temporarily disable all switches until the response is received
    obstacleSwitch.disabled = true;
    followSwitch.disabled = true;
    patrolSwitch.disabled = true;

    const isActive = activeSwitch.checked; // Save current state of the active switch
    const selectedMode = isActive ? mode : MODES.MANUAL;

    updateMode(selectedMode)
      .then(success => {
        if (success) {
          // Server responded with success, update the switch states
          inactiveSwitches.forEach(switchElement => switchElement.checked = false); // Turn off inactive switches
          activeSwitch.checked = isActive; // Keep the active switch checked
        } else {
          // If not successful, revert the switch state
          activeSwitch.checked = !isActive;
        }

        // Re-enable the switches
        obstacleSwitch.disabled = false;
        followSwitch.disabled = false;
        patrolSwitch.disabled = false;
      })
      .catch(error => {
        // Handle any errors and revert switch state
        console.error("Error:", error);
        activeSwitch.checked = !isActive; // Revert switch state if error

        // Re-enable the switches
        obstacleSwitch.disabled = false;
        followSwitch.disabled = false;
        patrolSwitch.disabled = false;
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