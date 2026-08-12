#pragma once

#include <QObject>

// File-level operations backing the Cut/Copy/Paste/Rename actions in the
// context menu. Deliberately thin for now - no progress reporting, no undo.
class FileOperations : public QObject
{
	Q_OBJECT

public:
	explicit FileOperations(QObject *parent = nullptr);

	// Copies sourcePath (file or directory, recursively) into destDir.
	Q_INVOKABLE bool copyPath(const QString &sourcePath, const QString &destDir);

	// Moves sourcePath into destDir. Tries an atomic rename first (same
	// filesystem); falls back to copy-then-delete across filesystems, e.g.
	// internal storage -> SD card.
	Q_INVOKABLE bool movePath(const QString &sourcePath, const QString &destDir);

	// Renames path in place to newName (no path separators allowed).
	Q_INVOKABLE bool renamePath(const QString &path, const QString &newName);

	// Number of entries directly inside path (not recursive), for the
	// "N items" subtitle on folder rows. Returns -1 if path can't be read.
	Q_INVOKABLE int entryCount(const QString &path) const;

Q_SIGNALS:
	void operationFailed(const QString &message);

private:
	bool copyRecursively(const QString &sourcePath, const QString &destPath);
};
