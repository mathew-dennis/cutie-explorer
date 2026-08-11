#pragma once

#include <QFileSystemWatcher>
#include <QObject>
#include <QVariantList>

// Tracks removable/external storage (USB drives, SD cards) for the sidebar's
// "Drives" section. There's no UDisks2/D-Bus integration here on purpose -
// the system's automount daemon (udisks2/gvfs) already does the mounting
// under /media, /run/media or /mnt, so this class just watches those roots
// and reports what QStorageInfo sees there.
class DriveManager : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QVariantList drives READ drives NOTIFY drivesChanged)

public:
	explicit DriveManager(QObject *parent = nullptr);

	QVariantList drives() const;

	// Callable from QML to force a re-read, e.g. on page open - mirrors
	// BatteryHistory.refresh()/PowerSaving.refresh() in Cutie.Battery.
	Q_INVOKABLE void refresh();

Q_SIGNALS:
	void drivesChanged();

private:
	void watchRoot(const QString &path);

	QFileSystemWatcher m_watcher;
	QVariantList m_drives;
};
