function updateStackInfo(taskName, usedStackPercentage) {
  const usedStackElement = document.getElementById(taskName);

  if (usedStackElement) {
    usedStackElement.innerText = `${usedStackPercentage.toFixed(2)}%`;
  }
}

window.updateStackInfo = updateStackInfo;