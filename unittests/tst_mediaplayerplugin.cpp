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

#include "aalmediaplayerservice.h"
#include "media_compatibility_layer.h"

#include <QObject>
#include <QMediaPlayerControl>
#include <QVideoRendererControl>
#include <QtTest/QtTest>

#define private public

struct MediaPlayerWrapper;

class tst_MediaPlayerPlugin : public QObject
{
    Q_OBJECT

    AalMediaPlayerService m_service;
    QMediaControl *m_playerControl;
    QMediaControl *m_rendererControl;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void tst_requestRelease();
    void tst_newMediaPlayer();
};

void tst_MediaPlayerPlugin::initTestCase()
{
    m_playerControl = m_service.requestControl(QMediaPlayerControl_iid);
    QVERIFY(m_playerControl != NULL);
    m_rendererControl = m_service.requestControl(QVideoRendererControl_iid);
    QVERIFY(m_rendererControl != NULL);
}

void tst_MediaPlayerPlugin::cleanupTestCase()
{
    m_service.releaseControl(m_playerControl);
    m_service.releaseControl(m_rendererControl);
}

void tst_MediaPlayerPlugin::tst_requestRelease()
{
    QMediaControl *mpControl = NULL;
    QMediaControl *rendererControl = NULL;

    // Request a new reference to the media player control and verify that
    // it's the same pointer as the original reference.
    mpControl = m_service.requestControl(QMediaPlayerControl_iid);
    QVERIFY(mpControl == m_playerControl);
    rendererControl = m_service.requestControl(QVideoRendererControl_iid);
    QVERIFY(rendererControl == m_rendererControl);

    // Now release the reference and make sure that the original media player
    // control reference still exists.
    m_service.releaseControl(mpControl);
    QVERIFY(mpControl == m_playerControl);
    m_service.releaseControl(rendererControl);
    QVERIFY(rendererControl == m_rendererControl);
}

void tst_MediaPlayerPlugin::tst_newMediaPlayer()
{
    bool ret = m_service.newMediaPlayer();
    QVERIFY(ret == true);
}

QTEST_MAIN(tst_MediaPlayerPlugin)

#include "tst_mediaplayerplugin.moc"
