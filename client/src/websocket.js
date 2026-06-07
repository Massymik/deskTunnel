let lastData = [];
let allFoldersOpen = false;

const fileIconPath = "./res/fileIcons/";

function getFileIcon(fileName) {
  const name = fileName.toLowerCase();
  if (name === "readme.md") return fileIconPath + "file_type_markdown.svg";
  if (name === ".gitignore") return fileIconPath + "file_type_git.svg";
  if (name === ".gitattributes") return fileIconPath + "file_type_git.svg";
  if (name === "cmakelists.txt") return fileIconPath + "file_type_cmake.svg";

  const lastDot = name.lastIndexOf(".");
  if (lastDot === -1) return "./res/fileIcon.svg";

  const ext = name.slice(lastDot + 1);
  return fileIconPath + "file_type_" + ext + ".svg";
}

function openAllFolders() {
  allFoldersOpen = !allFoldersOpen;

  const project = document.querySelector("contents");
  project.innerHTML = "";

  renderItems(lastData, project, 0, allFoldersOpen);

  const button = document.querySelector(".folderHeader img");
  if (button) {
    button.src = allFoldersOpen
      ? "./res/arrowDown.svg"
      : "./res/arrowRight.svg";
  }
}

const ws = new WebSocket("ws://localhost:9002/ws?key=ABC123");

ws.addEventListener("open", () => {
  ws.send("start");
});

ws.addEventListener("message", (event) => {
  const data = JSON.parse(event.data);

  if (data.type === "fileContents") {
    const contents = decodeBase64Utf8(data.contents);
    editor.setValue(contents);
    return;
  }

  let start = performance.now();
  lastData = data;

  const project = document.querySelector("contents");
  project.innerHTML = "";
  renderItems(lastData, project, 0, allFoldersOpen);

  let end = performance.now();
  console.log((end - start).toFixed(3) + "Ms");
});

function renderItems(items, parent, level, openAll = false) {
  items.sort((a, b) => {
    if (a.type === "folder" && b.type === "file") return -1;
    if (a.type === "file" && b.type === "folder") return 1;
    return a.name.localeCompare(b.name);
  });

  const levelColors = [
    "#e2e8f0",
    "#eab308",
    "#a855f7",
    "#ef4444",
  ];

  for (const item of items) {
    if (item.type !== "folder" && item.type !== "file") continue;

    const hasChildren =
      item.type === "folder" &&
      Array.isArray(item.children) &&
      item.children.length > 0;

    const element = document.createElement(item.type);

    if (item.type === "file") {
      element.setAttribute("path", item.path ?? "");
      element.addEventListener("click", (e) => {
        e.stopPropagation();
        const path = element.getAttribute("path");
        ws.send(JSON.stringify({
          operation: "getFileContents",
          path: path,
        }));
      });
    }

    element.style.display = "block";
    element.style.position = "relative";
    element.style.width = "100%";
    element.style.boxSizing = "border-box";

    if (item.type === "file") {
      const label = document.createElement("span");
      label.classList.add("fileLabelWithIcon");
      label.style.marginLeft = `${level * 10}px`;

      if (level > 0) {
        label.style.borderLeft = `1px solid ${
          levelColors[(level - 1) % levelColors.length]
        }`;
      }

      const icon = document.createElement("img");
      icon.classList.add("fileTypeIcon");
      icon.src = getFileIcon(item.name);
      icon.alt = "";

      icon.onerror = () => {
        icon.onerror = null;
        icon.src = "./res/fileIcon.svg";
      };

      const fileName = document.createElement("span");
      fileName.textContent = item.name;

      label.appendChild(icon);
      label.appendChild(fileName);
      element.appendChild(label);
    } else {
      const label = document.createElement("span");
      label.textContent = item.name;
      label.style.display = "inline-block";
      label.style.marginLeft = `${level * 10}px`;
      label.style.paddingLeft = "7px";

      if (level > 0) {
        label.style.borderLeft = `1px solid ${
          levelColors[(level - 1) % levelColors.length]
        }`;
      }

      element.appendChild(label);
    }

    if (level === 0 && item.type === "folder") {
      element.style.marginTop = "2px";
    }

    if (item.type === "folder") {
      element.style.fontWeight = "bold";
    }

    if (hasChildren) {
      const image = document.createElement("img");
      image.src = openAll ? "./res/arrowDown.svg" : "./res/arrowRight.svg";
      element.appendChild(image);
    }

    parent.appendChild(element);

    if (item.type === "folder" && hasChildren) {
      element.dataset.open = openAll ? "true" : "false";
      element.addEventListener("click", (e) => {
        e.stopPropagation();
        if (element.dataset.open === "true") {
          element.dataset.open = "false";

          const next = element.nextElementSibling;
          if (next && next.classList.contains("folderChildren")) {
            next.remove();
          }
          const img = element.querySelector("img");
          if (img) img.src = "./res/arrowRight.svg";
          return;
        }

        element.dataset.open = "true";
        const img = element.querySelector("img");
        if (img) img.src = "./res/arrowDown.svg";
        const childrenContainer = document.createElement("div");
        childrenContainer.className = "folderChildren";
        parent.insertBefore(childrenContainer, element.nextSibling);
        renderItems(item.children, childrenContainer, level + 1, false);
      });

      if (openAll) {
        const childrenContainer = document.createElement("div");
        childrenContainer.className = "folderChildren";

        parent.appendChild(childrenContainer);
        renderItems(item.children, childrenContainer, level + 1, true);
      }
    }
  }
}

ws.addEventListener("error", (error) => {
  console.log("WS error: " + error);
});

ws.addEventListener("close", () => {});