#include "drivemanager.h"

#include <QDir>
#include <QSet>
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

	// Android partitions that show up mounted under one of the watched
	// roots on this device but aren't "a drive the user plugged in" -
	// matched by name since which root they land under can vary.
	static const QSet<QString> kIgnoredNames = {
		QStringLiteral("persist"),
		QStringLiteral("efs"),
	};

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

		// /root and /userdata are fixed mounts on this device's own
		// storage layout rather than automounted removable media, but
		// they're wanted in the list too.
		const bool isExplicitRoot =
			rootPath == QLatin1String("/root")
			|| rootPath == QLatin1String("/userdata");

		if (!isRemovableRoot && !isExplicitRoot)
			continue;

		QString label = volume.displayName();
		if (label.isEmpty())
			label = rootPath.section(QLatin1Char('/'), -1);

		const QString pathTail = rootPath.section(QLatin1Char('/'), -1).toLower();
		if (kIgnoredNames.contains(pathTail) || kIgnoredNames.contains(label.toLower()))
			continue;

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
        m_drives = result;
        Q_EMIT drivesChanged();
    }
}