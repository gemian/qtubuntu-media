/*
 * Copyright (C) 2015 Canonical, Ltd.
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

#include "aalmediaplaylistcontrol.h"
#include "aalmediaplaylistprovider.h"

#include <core/signal.h>

#include <QEventLoop>
#include <QMediaPlaylist>

#include <QDebug>

// Uncomment for more verbose debugging to stdout/err
//#define VERBOSE_DEBUG

namespace media = core::ubuntu::media;

Q_DECLARE_METATYPE(core::ubuntu::media::Track::Id)

QT_BEGIN_NAMESPACE

namespace {
core::Signal<void> the_void;
}

AalMediaPlaylistControl::AalMediaPlaylistControl(QObject *parent)
    : QMediaPlaylistControl(parent),
      m_playlistProvider(nullptr),
      m_currentIndex(0),
      m_trackChangedConnection(the_void.connect([](){}))
{
    qDebug() << Q_FUNC_INFO;
    qRegisterMetaType<core::ubuntu::media::Track::Id>();
}

AalMediaPlaylistControl::~AalMediaPlaylistControl()
{
    qDebug() << Q_FUNC_INFO;
    disconnect_signals();
}

QMediaPlaylistProvider* AalMediaPlaylistControl::playlistProvider() const
{
    return m_playlistProvider;
}

bool AalMediaPlaylistControl::setPlaylistProvider(QMediaPlaylistProvider *playlist)
{
    m_playlistProvider = playlist;
    Q_EMIT playlistProviderChanged();
    return true;
}

int AalMediaPlaylistControl::currentIndex() const
{
    return m_currentIndex;
}

void AalMediaPlaylistControl::setCurrentIndex(int position)
{
    if (!m_hubTrackList) {
        qWarning() << "Track list does not exist so can't change current position";
        return;
    }

    qDebug() << Q_FUNC_INFO;
    const auto mediaCount = m_playlistProvider->mediaCount();
    qDebug() << "position: " << position << ", mediaCount: " << mediaCount;

    if (position < 0 || position >= mediaCount)
        return;

    qDebug() << "Going to position: " << position;

    try {
        const std::string id = aalMediaPlaylistProvider()->trackOfIndex(position);
        static const bool togglePlayerState = false;
        m_hubTrackList->go_to(id, togglePlayerState);
    }
    catch (const std::runtime_error &e) {
        qWarning() << "Failed to go to specified tracklist position: " << e.what();
    }
}

int AalMediaPlaylistControl::nextIndex(int steps) const
{
    const int x = m_currentIndex + steps;
    const int tracklistSize = m_playlistProvider->mediaCount();
#ifdef VERBOSE_DEBUG
    qDebug() << "m_currentIndex: " << m_currentIndex;
    qDebug() << "steps: " << steps;
    qDebug() << "tracklistSize: " << tracklistSize;
    qDebug() << "------------------------";
#endif
    if (x < tracklistSize)
        return x;
    else
        // algo: |(currentIndex + steps) - tracklist_size|
        return std::abs(x - tracklistSize);
}

int AalMediaPlaylistControl::previousIndex(int steps) const
{
    const int x  = m_currentIndex - steps;
    const int tracklistSize = m_playlistProvider->mediaCount();
    //const int reducedSteps = steps - ((steps / tracklistSize) * tracklistSize);
    // Calculate how many of x are in tracklistSize to reduce the calculation
    // to only wrap around the list one time
    const uint16_t m = (uint16_t)std::abs(x) / (uint16_t)tracklistSize; // 3
#ifdef VERBOSE_DEBUG
    qDebug() << "m_currentIndex: " << m_currentIndex;
    qDebug() << "steps: " << steps;
    qDebug() << "tracklistSize: " << tracklistSize;
    qDebug() << "x: " << x;
    qDebug() << "m: " << m;
    qDebug() << "------------------------";
#endif
    if (x >= 0)
        return x;
    else if (std::abs(x) > tracklistSize)
    {
        uint16_t i = m_currentIndex, stepCount = 0;
        bool doExit = false;
        while (!doExit)
        {
            if (i == 0)
                i = tracklistSize - 1;
            else
                --i;

            ++stepCount;
            if (stepCount == steps)
                doExit = true;
        }

        return i;
    }
    else
        return tracklistSize - std::abs(x);
}

void AalMediaPlaylistControl::next()
{
    qDebug() << Q_FUNC_INFO;

    try {
        m_hubPlayerSession->next();
    }
    catch (const std::runtime_error &e) {
        qWarning() << "Failed to go to next track: " << e.what();
    }
}

void AalMediaPlaylistControl::previous()
{
    qDebug() << Q_FUNC_INFO;

    try {
        m_hubPlayerSession->previous();
    }
    catch (const std::runtime_error &e) {
        qWarning() << "Failed to go to previous track: " << e.what();
    }
}

QMediaPlaylist::PlaybackMode AalMediaPlaylistControl::playbackMode() const
{
    QMediaPlaylist::PlaybackMode currentMode = QMediaPlaylist::Sequential;
    const auto loopStatus = m_hubPlayerSession->loop_status();
    switch (loopStatus)
    {
        case media::Player::LoopStatus::none:
            currentMode = QMediaPlaylist::Sequential;
            break;
        case media::Player::LoopStatus::track:
            currentMode = QMediaPlaylist::CurrentItemInLoop;
            break;
        case media::Player::LoopStatus::playlist:
            currentMode = QMediaPlaylist::Loop;
            break;
        default:
            qWarning() << "Unknown loop status: " << loopStatus;
    }

    // Shuffle overrides loopStatus since in the media-hub API random is not part of loop_status
    // like it's all one for QMediaPlaylist::PlaybackMode
    if (m_hubPlayerSession->shuffle())
        currentMode = QMediaPlaylist::Random;

    return currentMode;
}

void AalMediaPlaylistControl::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    qDebug() << Q_FUNC_INFO;
    switch (mode)
    {
        case QMediaPlaylist::CurrentItemOnce:
            qDebug() << "PlaybackMode: CurrentItemOnce";
            m_hubPlayerSession->shuffle() = false;
            qWarning() << "No media-hub equivalent for QMediaPlaylist::CurrentItemOnce";
            break;
        case QMediaPlaylist::CurrentItemInLoop:
            qDebug() << "PlaybackMode: CurrentItemInLoop";
            m_hubPlayerSession->shuffle() = false;
            m_hubPlayerSession->loop_status() = media::Player::LoopStatus::track;
            break;
        case QMediaPlaylist::Sequential:
            qDebug() << "PlaybackMode: Sequential";
            m_hubPlayerSession->shuffle() = false;
            m_hubPlayerSession->loop_status() = media::Player::LoopStatus::none;
            break;
        case QMediaPlaylist::Loop:
            qDebug() << "PlaybackMode: Loop";
            m_hubPlayerSession->shuffle() = false;
            m_hubPlayerSession->loop_status() = media::Player::LoopStatus::playlist;
            break;
        case QMediaPlaylist::Random:
            qDebug() << "PlaybackMode: Random";
            m_hubPlayerSession->shuffle() = true;
            break;
        default:
            qWarning() << "Unknown playback mode: " << mode;
            m_hubPlayerSession->shuffle() = false;
    }
}

void AalMediaPlaylistControl::setPlayerSession(const std::shared_ptr<core::ubuntu::media::Player>& playerSession)
{
    m_hubPlayerSession = playerSession;
    aalMediaPlaylistProvider()->setPlayerSession(playerSession);

    try {
        m_hubTrackList = m_hubPlayerSession->track_list();
    }
    catch (std::runtime_error &e) {
        qWarning() << "FATAL: Failed to retrieve the current player session TrackList: " << e.what();
    }

    connect_signals();
}

void AalMediaPlaylistControl::onTrackChanged(const core::ubuntu::media::Track::Id& id)
{
    if (!id.empty())
    {
        m_currentIndex = aalMediaPlaylistProvider()->indexOfTrack(id);
        qDebug() << "m_currentIndex updated to: " << m_currentIndex;
        const QMediaContent content = playlistProvider()->media(m_currentIndex);
        Q_EMIT currentMediaChanged(content);
        Q_EMIT currentIndexChanged(m_currentIndex);
    }
}

void AalMediaPlaylistControl::connect_signals()
{
    if (!m_hubTrackList) {
        qWarning() << "Can't connect to track list signals as it doesn't exist";
        return;
    }

    m_trackChangedConnection = m_hubTrackList->on_track_changed().connect([this](const media::Track::Id& id)
    {
        QMetaObject::invokeMethod(this, "onTrackChanged", Qt::QueuedConnection, Q_ARG(core::ubuntu::media::Track::Id, id));
    });
}

void AalMediaPlaylistControl::disconnect_signals()
{
    if (m_trackChangedConnection.is_connected())
        m_trackChangedConnection.disconnect();
}

AalMediaPlaylistProvider* AalMediaPlaylistControl::aalMediaPlaylistProvider()
{
    return static_cast<AalMediaPlaylistProvider*>(m_playlistProvider);
}

QT_END_NAMESPACE