function updateStackInfo(usedStackPercentage) {
  const usedStackElement = document.getElementById('websocket-usage');

  if (usedStackElement) {
    usedStackElement.innerText = `${usedStackPercentage.toFixed(2)}%`;
  }
}

window.updateStackInfo = updateStackInfo;