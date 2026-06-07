let currentLineIndx = 0;
const MAX_LINES = 13000;
const LINE_HEIGHT = 15;

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function getCursorLineIndex() {
  const selection = window.getSelection();
  if (!selection || selection.rangeCount === 0) {
    return currentLineIndx;
  }

  const range = selection.getRangeAt(0);
  const text = document.querySelector(".text");

  if (!text.contains(range.startContainer)) {
    return currentLineIndx;
  }

  const marker = document.createElement("span");
  marker.textContent = "\u200b";

  const clonedRange = range.cloneRange();
  clonedRange.collapse(true);
  clonedRange.insertNode(marker);

  const markerRect = marker.getBoundingClientRect();
  const editorRect = text.getBoundingClientRect();

  const y = markerRect.top - editorRect.top;

  marker.remove();

  selection.removeAllRanges();
  selection.addRange(range);

  return clamp(Math.floor(y / LINE_HEIGHT), 0, MAX_LINES - 1);
}

function updateCurrentLine() {
  const oldLine = document.querySelector("#linia" + currentLineIndx);
  if (oldLine) oldLine.classList.remove("currentLine");

  currentLineIndx = getCursorLineIndex();

  const newLine = document.querySelector("#linia" + currentLineIndx);
  if (newLine) newLine.classList.add("currentLine");
}

let code = document.querySelector(".code");
let text = document.createElement("div");
text.classList.add("text");
text.setAttribute("contenteditable", "true");
text.setAttribute("spellcheck", "false");
code.appendChild(text);
let lineNumberContainer = document.querySelector(".lineNumberContainer")

for(let i = 0; i < 13000; i++){
    let line = document.createElement("div");
    line.classList.add("line");
    line.id = "linia"+i;
    let lineNr = document.createElement("div");
    lineNr.classList.add("lineNr");
    lineNr.innerHTML = i+1;
    line.appendChild(lineNr); 
    if(i==currentLineIndx) line.classList.add("currentLine");
    lineNumberContainer.appendChild(line);
}

text.addEventListener("click", updateCurrentLine);
text.addEventListener("keyup", updateCurrentLine);
text.addEventListener("input", updateCurrentLine);