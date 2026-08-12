.pragma library

// Qt.labs.platform's StandardPaths returns file:// URLs, but everywhere
// else in this app (FolderListModel's folder, DriveManager's paths,
// FileOperations' arguments) works with plain filesystem paths - so
// anything coming from StandardPaths needs converting once, here.
function urlToPath(url) {
	return decodeURIComponent(url.toString().replace("file://", ""));
}

function humanSize(bytes) {
	if (bytes === undefined || bytes === null || isNaN(bytes))
		return "";

	var units = ["B", "KB", "MB", "GB", "TB"];
	var value = Math.max(0, bytes);
	var unitIndex = 0;

	while (value >= 1024 && unitIndex < units.length - 1) {
		value /= 1024;
		unitIndex++;
	}

	var decimals = (unitIndex === 0) ? 0 : 2;
	return value.toFixed(decimals) + units[unitIndex];
}

function formatDate(date) {
	if (!date)
		return "";
	return Qt.formatDateTime(date, "dd/MM/yy h:mm ap");
}
