import QtQuick

// Registered as the CutieExplorer.FileClipboard singleton in main.cpp.
// Just holds cut/copy state - the actual file move/copy happens in
// FileOperations (C++) when something is pasted.
QtObject {
	id: clipboard

	property string sourcePath: ""
	property string mode: ""          // "cut" | "copy" | ""
	readonly property bool hasContent: sourcePath.length > 0

	function cut(path) {
		clipboard.sourcePath = path;
		clipboard.mode = "cut";
	}

	function copy(path) {
		clipboard.sourcePath = path;
		clipboard.mode = "copy";
	}

	function clear() {
		clipboard.sourcePath = "";
		clipboard.mode = "";
	}
}
