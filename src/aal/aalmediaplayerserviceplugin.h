/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AALMEDIAPLAYERSERVICEPLUGIN_H
#define AALMEDIAPLAYERSERVICEPLUGIN_H

#include <QMediaServiceProviderPlugin>

class AalServicePlugin : public QMediaServiceProviderPlugin,
                         public QMediaServiceSupportedDevicesInterface
{
    Q_OBJECT
    Q_INTERFACES(QMediaServiceSupportedDevicesInterface)
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.mediaserviceproviderfactory/5.0" FILE "aalmediaplayer.json")

public:
    AalServicePlugin();

    QMediaService* create(QString const& key);
    void release(QMediaService *service);

    QList<QByteArray> devices(const QByteArray &service) const;
    QString deviceDescription(const QByteArray &service, const QByteArray &device);
};

#endif
