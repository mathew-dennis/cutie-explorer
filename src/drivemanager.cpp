#include "drivemanager.h"

#include <QDir>
#include <QStorageInfo>

namespace {
// Where Linux automount daemons (udisks2/gvfs) place removable media.
const char *kWatchRoots[] = {"/media", "/run/media", "/mnt"};
}

DriveManager::DriveManager(QObject *parent)
	: QObject(parent)
{
	for (const char *root : kWatchRoots)
		watchRoot(QString::fromLatin1(root));

	connect(&m_watcher, &QFileSystemWatcher::directoryChanged,
		this, &DriveManager::refresh);

	refresh();
}

void DriveManager::watchRoot(const QString &path)
{
	QDir dir(path);
	if (!dir.exists())
		return;

	m_watcher.addPath(path);

	// The root itself (e.g. /media) rarely changes - it's the per-user
	// subfolder (/media/mathew, /run/media/mathew) that actually gains or
	// loses an entry when a drive is mounted or unmounted, so watch those
	// too.
	const auto entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString &entry : entries)
		m_watcher.addPath(dir.filePath(entry));
}

QVariantList DriveManager::drives() const
{
	return m_drives;
}

void DriveManager::refresh()
{
	QVariantList result;

	const auto volumes = QStorageInfo::mountedVolumes();
	for (const QStorageInfo &volume : volumes) {
		if (!volume.isValid() || !volume.isReady())
			continue;

		const QString rootPath = volume.rootPath();

		// Only surface volumes mounted under the conventional removable-
		// media roots - excludes the root filesystem, /boot, tmpfs and
		// other mounts that aren't "a drive the user plugged in".
		const bool isRemovableRoot =
			rootPath.startsWith(QLatin1String("/media/"))
			|| rootPath.startsWith(QLatin1String("/run/media/"))
			|| rootPath.startsWith(QLatin1String("/mnt/"));
		if (!isRemovableRoot)
			continue;

		QString label = volume.displayName();
		if (label.isEmpty())
			label = rootPath.section(QLatin1Char('/'), -1);

		QVariantMap entry;
		entry["name"] = label;
		entry["path"] = rootPath;
		entry["totalBytes"] = volume.bytesTotal();
		entry["freeBytes"] = volume.bytesAvailable();
		result.append(entry);
	}

	if (result != m_drives) {
		m_drives = result;
		Q_EMIT drivesChanged();
	}
}
