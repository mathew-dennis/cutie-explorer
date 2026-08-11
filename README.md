# Cutie Explorer

a very minimal file explorer for cutie shell 

<img src="cutie-explorer.svg" width="100px">

A file manager for Cutie Shell. Sidebar with quick-access places (Home,
Desktop, Documents, Downloads, Pictures, Music, Videos) and detected
external drives/SD cards, a breadcrumb path bar, list/grid views, and a
Cut/Copy/Paste/Rename/Properties context menu per item.

## Building and installing

```
mkdir build
cd build
cmake ..
make
sudo make install
```

## Architecture

- Directory listing is `Qt.labs.folderlistmodel.FolderListModel` - no
  custom C++ needed, live-updates when the directory changes on disk.
- `DriveManager` (C++ singleton) watches `/media`, `/run/media`, `/mnt`
  with `QFileSystemWatcher` and reports mounted volumes there via
  `QStorageInfo`. No UDisks2/D-Bus - the automount daemon already does the
  mounting, this just reports what shows up.
- `FileOperations` (C++ singleton) does the actual copy/move/rename with
  `QFile`/`QDir`. Move tries an atomic rename first, falls back to
  copy-then-delete across filesystems (e.g. internal storage -> SD card).
- `FileClipboard` is a plain QML singleton (`FileClipboard.qml`, registered
  from a URL rather than `pragma Singleton`) holding just the cut/copy
  source path - no filesystem access of its own.
- View mode (list/grid) persists via `Qt.labs.settings.Settings`.

## Things to verify when you compile

- `FolderListModel` role names (`fileName`, `filePath`, `fileSize`,
  `fileIsDir`, `fileModified`, ...) - used from memory, not checked against
  your installed Qt6 version.
- `image://theme/<name>` for grid-view icons - relies on Qt Quick's built-in
  icon theme provider; if it doesn't resolve, swap for `icon.name` on a
  `CutieButton` with `background: null` instead (same trick used elsewhere
  in Cutie Settings).
- Package names for `qml6-module-qt-labs-folderlistmodel`,
  `qml6-module-qt-labs-platform`, `qml6-module-qt-labs-settings` in
  `debian/control` - guessed from the `qml6-module-cutie*` naming pattern,
  worth an `apt-cache search qml6-module-qt-labs` to confirm.
- Breadcrumb navigation re-pushes a page for earlier segments instead of
  popping the stack back to it - correct but not the most economical; fine
  to leave as-is unless it's noticeably wrong in practice.

## Not yet implemented

- Delete (only Cut/Copy/Paste/Rename/Properties, as asked for).
- New folder / new file.
- Multi-select.
- Per-mimetype icons (folders vs "generic file" only, for now).

## Troubleshooting
wip
