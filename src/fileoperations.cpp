#include "fileoperations.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

FileOperations::FileOperations(QObject *parent)
	: QObject(parent)
{
}

bool FileOperations::copyRecursively(const QString &sourcePath, const QString &destPath)
{
	QFileInfo sourceInfo(sourcePath);

	if (sourceInfo.isDir()) {
		QDir destDir(destPath);
		if (!destDir.mkpath(destPath)) {
			Q_EMIT operationFailed(tr("Could not create %1").arg(destPath));
			return false;
		}

		QDir sourceDir(sourcePath);
		const auto entries = sourceDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
		for (const QString &entry : entries) {
			if (!copyRecursively(sourceDir.filePath(entry), destDir.filePath(entry)))
				return false;
		}
		return true;
	}

	if (QFile::exists(destPath))
		QFile::remove(destPath);

	if (!QFile::copy(sourcePath, destPath)) {
		Q_EMIT operationFailed(tr("Could not copy %1").arg(sourcePath));
		return false;
	}
	return true;
}

bool FileOperations::copyPath(const QString &sourcePath, const QString &destDir)
{
	QFileInfo sourceInfo(sourcePath);
	if (!sourceInfo.exists()) {
		Q_EMIT operationFailed(tr("%1 no longer exists").arg(sourcePath));
		return false;
	}

	const QString destPath = QDir(destDir).filePath(sourceInfo.fileName());
	if (destPath == sourcePath) {
		Q_EMIT operationFailed(tr("Source and destination are the same"));
		return false;
	}

	return copyRecursively(sourcePath, destPath);
}

bool FileOperations::movePath(const QString &sourcePath, const QString &destDir)
{
	QFileInfo sourceInfo(sourcePath);
	if (!sourceInfo.exists()) {
		Q_EMIT operationFailed(tr("%1 no longer exists").arg(sourcePath));
		return false;
	}

	const QString destPath = QDir(destDir).filePath(sourceInfo.fileName());
	if (destPath == sourcePath)
		return true;

	QDir dir;
	if (dir.rename(sourcePath, destPath))
		return true;

	if (!copyRecursively(sourcePath, destPath))
		return false;

	if (sourceInfo.isDir())
		return QDir(sourcePath).removeRecursively();
	return QFile::remove(sourcePath);
}

bool FileOperations::renamePath(const QString &path, const QString &newName)
{
	if (newName.isEmpty() || newName.contains(QLatin1Char('/'))) {
		Q_EMIT operationFailed(tr("Invalid name"));
		return false;
	}

	QFileInfo info(path);
	const QString destPath = info.absoluteDir().filePath(newName);

	QDir dir;
	if (!dir.rename(path, destPath)) {
		Q_EMIT operationFailed(tr("Could not rename to %1").arg(newName));
		return false;
	}
	return true;
}

int FileOperations::entryCount(const QString &path) const
{
	QDir dir(path);
	if (!dir.exists())
		return -1;
	return dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).count();
}
