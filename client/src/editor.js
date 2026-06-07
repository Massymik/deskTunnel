let editor = null;
let currentModel = null;
let currentFilePath = null;
let currentFileName = null;

document.addEventListener("DOMContentLoaded", () => {
require.config({
    paths: {
        vs: "https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.53.0/min/vs"
    }
});

require(["vs/editor/editor.main"], function () {
    monaco.editor.defineTheme("deskTunnel-dark", {
        base: "vs-dark",
        inherit: true,
        rules: [
            { token: "comment", foreground: "6A9955" },
            { token: "string", foreground: "CE9178" },
            { token: "number", foreground: "B5CEA8" },
            { token: "keyword", foreground: "569CD6" },
            { token: "operator", foreground: "D4D4D4" },
            { token: "type", foreground: "4EC9B0" },
            { token: "type.identifier", foreground: "4EC9B0" },
            { token: "identifier", foreground: "D4D4D4" },
            { token: "identifier.function", foreground: "DCDCAA" },
            { token: "predefined", foreground: "4EC9B0" },
            { token: "namespace", foreground: "4EC9B0" },
            { token: "keyword.directive", foreground: "C586C0" },
            { token: "keyword.directive.include", foreground: "C586C0" },
            { token: "string.include", foreground: "CE9178" },
            { token: "tag", foreground: "569CD6" },
            { token: "attribute.name", foreground: "9CDCFE" },
            { token: "attribute.value", foreground: "CE9178" },
            { token: "delimiter.html", foreground: "808080" },
            { token: "metatag", foreground: "569CD6" },
            { token: "key", foreground: "9CDCFE" },
            { token: "variable", foreground: "9CDCFE" },
            { token: "variable.predefined", foreground: "4FC1FF" },
            { token: "property", foreground: "9CDCFE" },
            { token: "type.css", foreground: "4EC9B0" },
            { token: "attribute.value.css", foreground: "CE9178" },
            { token: "delimiter", foreground: "D4D4D4" },
            { token: "delimiter.bracket", foreground: "D4D4D4" },
            { token: "delimiter.parenthesis", foreground: "D4D4D4" },
            { token: "delimiter.square", foreground: "D4D4D4" },
            { token: "delimiter.curly", foreground: "D4D4D4" }
        ],
        colors: {
            "editor.background": "#1e1e1e",
            "editor.foreground": "#d4d4d4",
            "editorLineNumber.foreground": "#858585",
            "editorLineNumber.activeForeground": "#c6c6c6",
            "editorCursor.foreground": "#aeafad",
            "editor.selectionBackground": "#264f78",
            "editor.inactiveSelectionBackground": "#3a3d41",
            "editor.selectionHighlightBackground": "#add6ff26",
            "editor.lineHighlightBackground": "#2a2d2e",
            "editor.lineHighlightBorder": "#00000000",
            "editorIndentGuide.background1": "#404040",
            "editorIndentGuide.activeBackground1": "#707070",
            "editorBracketMatch.background": "#0064001a",
            "editorBracketMatch.border": "#888888",
            "editorGutter.background": "#1e1e1e",
            "editorOverviewRuler.border": "#00000000",
            "minimap.background": "#1e1e1e",
            "scrollbarSlider.background": "#79797966",
            "scrollbarSlider.hoverBackground": "#646464b3",
            "scrollbarSlider.activeBackground": "#bfbfbf66"
        }
    });

    editor = monaco.editor.create(document.getElementById("monacoEditor"), {
        value: "",
        language: "cpp",
        theme: "deskTunnel-dark",
        automaticLayout: true,
        fontSize: 14,
        lineHeight: 20,
        fontFamily: "Consolas, 'Courier New', monospace",
        minimap: {
            enabled: true,
            renderCharacters: false,
            maxColumn: 120
        },
        scrollBeyondLastLine: false,
        smoothScrolling: true,
        cursorSmoothCaretAnimation: "on",
        fastScrollSensitivity: 5,
        mouseWheelScrollSensitivity: 2,
        roundedSelection: false,
        renderLineHighlight: "line",
        overviewRulerBorder: false,
        bracketPairColorization: {
            enabled: true
        },
        guides: {
            bracketPairs: true,
            indentation: true
        },
        tabSize: 2,
        insertSpaces: true,
        detectIndentation: true,
        wordWrap: "off",
        renderWhitespace: "selection",
        renderControlCharacters: false,
        suggestOnTriggerCharacters: true,
        quickSuggestions: true,
        parameterHints: {
            enabled: true
        }
    });

    editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyS, function () {
        saveCurrentFile();
    });
});
});

function decodeBase64Utf8(base64) {
  const binary = atob(base64);
  const bytes = Uint8Array.from(binary, char => char.charCodeAt(0));
  return new TextDecoder("utf-8").decode(bytes);
}

function detectLanguage(filename) {
    if (!filename) return "plaintext";

    const ext = filename.split(".").pop().toLowerCase();

    switch (ext) {
        case "cpp":
        case "cc":
        case "cxx":
        case "c++":
        case "hpp":
        case "hh":
        case "hxx":
        case "h":
            return "cpp";

        case "c":
            return "c";

        case "js":
        case "mjs":
        case "cjs":
            return "javascript";

        case "ts":
            return "typescript";

        case "jsx":
            return "javascript";

        case "tsx":
            return "typescript";

        case "html":
        case "htm":
            return "html";

        case "css":
            return "css";

        case "scss":
            return "scss";

        case "less":
            return "less";

        case "php":
            return "php";

        case "json":
            return "json";

        case "jsonc":
            return "json";

        case "md":
        case "markdown":
            return "markdown";

        case "xml":
        case "svg":
            return "xml";

        case "py":
            return "python";

        case "java":
            return "java";

        case "cs":
            return "csharp";

        case "sql":
            return "sql";

        case "sh":
        case "bash":
            return "shell";

        case "bat":
        case "cmd":
            return "bat";

        case "ps1":
            return "powershell";

        case "yaml":
        case "yml":
            return "yaml";

        case "toml":
        case "ini":
        case "env":
            return "ini";

        default:
            return "plaintext";
    }
}

function setEditorContent(content, filename = "file.txt", path = null) {
    if (!editor) return;

    currentFileName = filename;
    currentFilePath = path || filename;

    const language = detectLanguage(filename);

    if (currentModel) {
        currentModel.dispose();
    }

    currentModel = monaco.editor.createModel(content, language);
    editor.setModel(currentModel);
    editor.focus();
}

function getEditorContent() {
    if (!editor) return "";
    return editor.getValue();
}

function getCurrentFilePath() {
    return currentFilePath;
}

function getCurrentFileName() {
    return currentFileName;
}

function saveCurrentFile() {
    if (!currentFilePath)
        return;
    const content = getEditorContent();
    if (typeof ws !== "undefined" && ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({
            action: "saveFile",
            path: currentFilePath,
            name: currentFileName,
            content: content
        }));
        return;
    }
}

function openFileInEditor(file) {
    if (!file) return;

    const name = file.name || file.filename || "file.txt";
    const path = file.path || file.fullPath || name;
    const content = file.content || "";

    setEditorContent(content, name, path);
}

function setEditorLanguage(filename) {
    if (!editor || !currentModel) return;

    const language = detectLanguage(filename);
    monaco.editor.setModelLanguage(currentModel, language);
}

setEditorContent("no opened files yet")