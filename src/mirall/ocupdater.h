/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#ifndef OCUPDATER_H
#define OCUPDATER_H

#include <QObject>
#include <QUrl>
#include <QTemporaryFile>

#include "mirall/updateinfo.h"
#include "mirall/updater.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace Mirall {

/** @short Class that uses an ownCloud propritary XML format to fetch update information */
class OCUpdater : public QObject, public Updater
{
    Q_OBJECT
public:
    enum DownloadState { Unknown = 0, UpToDate, Downloading, DownloadComplete,
                         DownloadFailed, UpdateOnlyAvailableThroughSystem };
    explicit OCUpdater(const QUrl &url, QObject *parent = 0);

    void performUpdate();

    void checkForUpdate();
    void backgroundCheckForUpdate();

    QString statusString() const;
    int downloadState() const;
    void setDownloadState(DownloadState state);

signals:
    void downloadStateChanged();

public slots:
    void slotStartInstaller();

private slots:
    void slotOpenUpdateUrl();
    void slotSetVersionSeen();
    void slotVersionInfoArrived();

protected:
    virtual void versionInfoArrived(const UpdateInfo &info) = 0;
    bool updateSucceeded() const;
    QString clientVersion() const;
    QString getSystemInfo();
    QNetworkAccessManager* qnam() const { return _accessManager; }

private:
    QUrl _updateUrl;
    int _state;
    QNetworkAccessManager *_accessManager;
    UpdateInfo _updateInfo;
};

/** Windows Updater Using NSIS */
class NSISUpdater : public OCUpdater {
    Q_OBJECT
public:
    enum UpdateState { NoUpdate = 0, UpdateAvailable, UpdateFailed };
    explicit NSISUpdater(const QUrl &url, QObject *parent = 0);
    bool handleStartup();
private slots:
    void slotDownloadFinished();
    void slotWriteFile();
private:
    NSISUpdater::UpdateState updateState() const;
    void showDialog(const UpdateInfo &info);
    void showFallbackMessage();
    void versionInfoArrived(const UpdateInfo &info);
    QScopedPointer<QTemporaryFile> _file;
    QString _targetFile;
    bool _showFallbackMessage;

};

/** Passive updater: Only implements notification for use in settings.
    Does not do popups */
class PassiveUpdateNotifier : public OCUpdater {
    Q_OBJECT
public:
    explicit PassiveUpdateNotifier(const QUrl &url, QObject *parent = 0);
    bool handleStartup() { return false; }

private:
    void versionInfoArrived(const UpdateInfo &info);
};



}

#endif // OC_UPDATER