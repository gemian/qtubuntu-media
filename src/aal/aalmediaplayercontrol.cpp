/*
 * Copyright (C) 2013-2014 Canonical, Ltd.
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

#include "aalmediaplayercontrol.h"
#include "aalmediaplayerservice.h"
#include "aalvideorenderercontrol.h"

#include <media/media_compatibility_layer.h>

#include <QMediaPlaylist>
#include <QDebug>
#include <QApplication>
#include <QTimer>

AalMediaPlayerControl::AalMediaPlayerControl(AalMediaPlayerService *service, QObject *parent)
   : QMediaPlayerControl(parent),
    m_service(service),
    m_state(QMediaPlayer::StoppedState),
    m_status(QMediaPlayer::NoMedia),
    m_applicationActive(true),
    m_cachedSeek(0),
    m_allowSeek(true)
{
    m_cachedVolume = volume();

    QApplication::instance()->installEventFilter(this);
}

AalMediaPlayerControl::~AalMediaPlayerControl()
{
    stop();
    m_state = QMediaPlayer::StoppedState;
    m_status = QMediaPlayer::NoMedia;
    m_cachedVolume = 0;
}

bool AalMediaPlayerControl::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ApplicationDeactivate) {
       m_applicationActive = false;
       m_service->pushPlaylist();
    }
    else if (event->type() == QEvent::ApplicationActivate) {
       m_applicationActive = true;
    }

    return QObject::eventFilter(obj, event);
}

QMediaPlayer::State AalMediaPlayerControl::state() const
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    return m_state;
}

QMediaPlayer::MediaStatus AalMediaPlayerControl::mediaStatus() const
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    return m_status;
}

qint64 AalMediaPlayerControl::duration() const
{
    return m_service->duration();
}

qint64 AalMediaPlayerControl::position() const
{
    return m_service->position();
}

void AalMediaPlayerControl::debounceSeek()
{
    m_allowSeek = true;
}

void AalMediaPlayerControl::setPosition(qint64 msec)
{
    m_cachedSeek = msec;
    // The actual debouncing
    if (!m_allowSeek)
        return;

    // Debounce seek requests with this single shot timer every 250 ms period
    QTimer::singleShot(250, this, SLOT(debounceSeek()));

    m_service->setPosition(msec);
    Q_EMIT positionChanged(msec);

    // Protect from another setPosition until the timer expires
    m_allowSeek = false;
}

int AalMediaPlayerControl::volume() const
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    return m_service->getVolume();
}

void AalMediaPlayerControl::setVolume(int volume)
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    m_cachedVolume = volume;
    m_service->setVolume(volume);
    Q_EMIT volumeChanged(m_cachedVolume);
}

bool AalMediaPlayerControl::isMuted() const
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    return (volume() == 0);
}

void AalMediaPlayerControl::setMuted(bool muted)
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    if (muted)
    {
        m_cachedVolume = volume();
        setVolume(0);
    }
    else
    {
        setVolume(m_cachedVolume);
    }

    Q_EMIT mutedChanged(muted);
}

int AalMediaPlayerControl::bufferStatus() const
{
    // Until we are playing network streams, there is no buffering necessary
    return 100;
}

bool AalMediaPlayerControl::isAudioAvailable() const
{
    return m_service->isAudioSource();
}

bool AalMediaPlayerControl::isVideoAvailable() const
{
    return m_service->isVideoSource();
}

bool AalMediaPlayerControl::isSeekable() const
{
    return true;
}

QMediaTimeRange AalMediaPlayerControl::availablePlaybackRanges() const
{
    // TODO: this will need to change once we can play networked sources
    return QMediaTimeRange(0, duration());
}

qreal AalMediaPlayerControl::playbackRate() const
{
    return 1.0;
}

void AalMediaPlayerControl::setPlaybackRate(qreal rate)
{
    Q_UNUSED(rate);
}

QMediaContent AalMediaPlayerControl::media() const
{
    return m_mediaContent;
}

const QIODevice* AalMediaPlayerControl::mediaStream() const
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    // This is only valid if a stream was passed into setMedia()
    return NULL;
}

void AalMediaPlayerControl::setMedia(const QMediaContent& media, QIODevice* stream)
{
    Q_UNUSED(stream);
    qDebug() << __PRETTY_FUNCTION__ << endl;
    QMediaPlaylist *list;

    if (stream != NULL) {
        try
        {
            list = reinterpret_cast<QMediaPlaylist*>(stream);
            m_service->setMediaPlaylist(*list);

            // Stream is a QMediaPlaylist object
            m_mediaContent = QMediaContent(list);
        }
        catch (const std::bad_cast &e)
        {
            // TODO: Support real streams
            qDebug() << "Streams are not currently supported";
            stop();
            return;
        }
    } else {
        m_mediaContent = media;

        // Make sure we can actually load something valid
        if (!media.isNull())
        {
            setMediaStatus(QMediaPlayer::LoadingMedia);
            m_service->setMedia(unescape(media));
        }
    }
    Q_EMIT mediaChanged(m_mediaContent);
}

void AalMediaPlayerControl::play()
{
    qDebug() << __PRETTY_FUNCTION__ << endl;

    m_allowSeek = true;
    setPosition(m_cachedSeek);
    // Make sure that a single setPosition works after this previous setPosition
    m_allowSeek = true;

    setState(QMediaPlayer::PlayingState);
    m_service->play();
}

void AalMediaPlayerControl::pause()
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    m_service->pause();

    setState(QMediaPlayer::PausedState);
}

void AalMediaPlayerControl::stop()
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    m_service->stop();

    setState(QMediaPlayer::StoppedState);
}

void AalMediaPlayerControl::playbackComplete()
{
    qDebug() << __PRETTY_FUNCTION__ << endl;
    setMediaStatus(QMediaPlayer::EndOfMedia);
}

void AalMediaPlayerControl::mediaPrepared()
{
    setMediaStatus(QMediaPlayer::LoadedMedia);
    Q_EMIT durationChanged(duration());
    Q_EMIT positionChanged(position());
}

void AalMediaPlayerControl::emitDurationChanged(qint64 duration)
{
    Q_EMIT durationChanged(duration);
}

QUrl AalMediaPlayerControl::unescape(const QMediaContent &media) const
{
    if (media.isNull())
        return QUrl();

    return QUrl::fromPercentEncoding(media.canonicalUrl().toString().toUtf8());
}

void AalMediaPlayerControl::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (status != m_status)
    {
        m_status = status;
        Q_EMIT mediaStatusChanged(m_status);
    }
}

void AalMediaPlayerControl::setState(QMediaPlayer::State state)
{
    if (state != m_state)
    {
        m_state = state;
        Q_EMIT stateChanged(m_state);
    }
}