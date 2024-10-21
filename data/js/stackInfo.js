function updateStackInfo(freeStackPercentage, usedStackPercentage) {
  const freeStackElement = document.getElementById('free-stack');
  const usedStackElement = document.getElementById('stack-usage');

  if (freeStackElement && usedStackElement) {
    freeStackElement.innerText = `${freeStackPercentage.toFixed(2)}%`;
    usedStackElement.innerText = `${usedStackPercentage.toFixed(2)}%`;
  }
}

window.updateStackInfo = updateStackInfo;