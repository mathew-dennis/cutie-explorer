import Cutie
import CutieExplorer
import Qt.labs.platform as Labs
import QtQuick
import "Formatting.js" as Formatting

CutieWindow {
	id: mainWindow
	width: 400
	height: 800
	visible: true
	title: qsTr("Files")

	property var folderComponent: Qt.createComponent("FolderView.qml")

	// Quick-access shortcuts - the standard XDG user directories, resolved
	// at runtime rather than hardcoded so they still work if the user has
	// remapped them in user-dirs.dirs.
	property var places: [
		{ text: qsTr("Home"), icon: "user-home-symbolic", path: Formatting.urlToPath(Labs.StandardPaths.writableLocation(Labs.StandardPaths.HomeLocation)) },
		{ text: qsTr("Desktop"), icon: "user-desktop-symbolic", path: Formatting.urlToPath(Labs.StandardPaths.writableLocation(Labs.StandardPaths.DesktopLocation)) },
		{ text: qsTr("Documents"), icon: "folder-documents-symbolic", path: Formatting.urlToPath(Labs.StandardPaths.writableLocation(Labs.StandardPaths.DocumentsLocation)) },
		{ text: qsTr("Downloads"), icon: "folder-download-symbolic", path: Formatting.urlToPath(Labs.StandardPaths.writableLocation(Labs.StandardPaths.DownloadLocation)) },
		{ text: qsTr("Pictures"), icon: "folder-pictures-symbolic", path: Formatting.urlToPath(Labs.StandardPaths.writableLocation(Labs.StandardPaths.PicturesLocation)) },
		{ text: qsTr("Music"), icon: "folder-music-symbolic", path: Formatting.urlToPath(Labs.StandardPaths.writableLocation(Labs.StandardPaths.MusicLocation)) },
		{ text: qsTr("Videos"), icon: "folder-videos-symbolic", path: Formatting.urlToPath(Labs.StandardPaths.writableLocation(Labs.StandardPaths.MoviesLocation)) }
	]

	function openFolder(path, label) {
		if (mainWindow.folderComponent.status === Component.Ready) {
			mainWindow.pageStack.push(mainWindow.folderComponent,
				{ crumbs: [{ label: label, path: path }] });
		}
	}

	initialPage: CutiePage {
		width: mainWindow.width
		height: mainWindow.height

		Flickable {
			anchors.fill: parent
			contentHeight: column.height

			Column {
				id: column
				width: parent.width

				CutiePageHeader {
					title: mainWindow.title
					width: parent.width
				}

				Repeater {
					model: mainWindow.places
					delegate: CutieListItem {
						width: column.width
						text: mainWindow.places[index]["text"]
						icon.name: mainWindow.places[index]["icon"]
						icon.color: Atmosphere.textColor
						onClicked: mainWindow.openFolder(
							mainWindow.places[index]["path"],
							mainWindow.places[index]["text"]);
					}
				}

				// ── Drives ──────────────────────────────────────────────
				// Populated by DriveManager, which only reports volumes
				// mounted under /media, /run/media or /mnt - i.e. things
				// the user actually plugged in, not the root filesystem.
				CutieLabel {
					text: qsTr("Drives")
					visible: DriveManager.drives.length > 0
					leftPadding: 20
					topPadding: 20
					bottomPadding: 6
					font.pixelSize: 13
					font.bold: true
					opacity: 0.6
				}

				Repeater {
					model: DriveManager.drives
					delegate: CutieListItem {
						width: column.width
						text: modelData.name
						subText: qsTr("%1 free of %2")
							.arg(Formatting.humanSize(modelData.freeBytes))
							.arg(Formatting.humanSize(modelData.totalBytes))
						icon.name: "drive-removable-media-symbolic"
						icon.color: Atmosphere.textColor
						onClicked: mainWindow.openFolder(modelData.path, modelData.name)
					}
				}

				Item { width: 1; height: 16 }
			}
		}
	}
}
