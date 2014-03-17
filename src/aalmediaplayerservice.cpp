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

#include <errno.h>

#include <QAbstractVideoSurface>
#include <qgl.h>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QGLContext>
#include <QtQuick/QQuickWindow>

#include <QDebug>

namespace media = core::ubuntu::media;

enum {
    OK          = 0,
    NO_ERROR    = 0,
    BAD_VALUE   = -EINVAL,
};

AalMediaPlayerService *AalMediaPlayerService::m_service = 0;

AalMediaPlayerService::AalMediaPlayerService(QObject *parent):
    QMediaService(parent),
    m_videoOutputReady(false),
    m_offscreenSurface(0),
    m_context(0),
    m_glContext(0),
    m_mediaPlayerControlRef(0),
    m_videoOutputRef(0),
    m_setVideoSizeCb(0),
    m_setVideoSizeContext(0)
{
    m_service = this;

    m_hubService = media::Service::Client::instance();

    if (!newMediaPlayer())
        qWarning() << "Failed to create a new media player backend. Video playback will not function." << endl;

    m_videoOutput = new AalVideoRendererControl(this);
    m_mediaPlayerControl = new AalMediaPlayerControl(this);

    // Get notified when the application window is displayed and focused
    //connect(QGuiApplication::instance(), SIGNAL(focusWindowChanged(QWindow*)), this,
    //        SLOT(handleFocusWindowChanged(QWindow*)), Qt::QueuedConnection);
}

AalMediaPlayerService::~AalMediaPlayerService()
{
    if (m_mediaPlayerControl != NULL)
        delete m_mediaPlayerControl;
    if (m_videoOutput != NULL)
        delete m_videoOutput;
#if 0
    if (m_androidMediaPlayer != NULL)
        delete m_androidMediaPlayer;
#endif
}

QMediaControl *AalMediaPlayerService::requestControl(const char *name)
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
    {
        if (m_mediaPlayerControlRef == 0 && m_mediaPlayerControl == NULL)
            m_mediaPlayerControl = new AalMediaPlayerControl(this);

        ++m_mediaPlayerControlRef;
        return m_mediaPlayerControl;
    }

    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
    {
        if (m_videoOutputRef == 0 && m_videoOutput == NULL)
            m_videoOutput = new AalVideoRendererControl(this);

        ++m_videoOutputRef;
        return m_videoOutput;
    }

    return NULL;
}

void AalMediaPlayerService::releaseControl(QMediaControl *control)
{
    if (control == m_mediaPlayerControl)
    {
        if (m_mediaPlayerControlRef > 0)
            --m_mediaPlayerControlRef;

        if (m_mediaPlayerControlRef == 0)
        {
            if (m_mediaPlayerControl != NULL)
            {
                delete m_mediaPlayerControl;
                m_mediaPlayerControl = NULL;
                control = NULL;
            }
        }
    }
    else if (control == m_videoOutput)
    {
        if (m_videoOutputRef > 0)
            --m_videoOutputRef;

        if (m_videoOutputRef == 0)
        {
            if (m_videoOutput != NULL)
            {
                delete m_videoOutput;
                m_videoOutput = NULL;
                control = NULL;
            }
        }
    }
}

MediaPlayerWrapper *AalMediaPlayerService::androidControl()
{
#if 0
    return m_androidMediaPlayer;
#endif
    return NULL;
}

AalMediaPlayerService::GLConsumerWrapperHybris AalMediaPlayerService::glConsumer() const
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot return glConsumer without a valid media-hub player session";
        return NULL;
    }
    return m_hubPlayerSession->gl_consumer();
}

bool AalMediaPlayerService::newMediaPlayer()
{
#if 0
    if (m_androidMediaPlayer)
        return true;

    m_androidMediaPlayer = android_media_new_player();
    if (!m_androidMediaPlayer) {
        qWarning() << "Unable to create a new media player instance.";
        return false;
    }
#endif

    // Only one player session needed
    if (m_hubPlayerSession)
        return true;

    try {
        m_hubPlayerSession = m_hubService->create_session(media::Player::Client::default_configuration());

    }
    catch (std::runtime_error &e) {
        qWarning() << "Failed to start a new media-hub player session: " << e.what();
        return false;
    }

    return true;
}

void AalMediaPlayerService::setupMediaPlayer()
{
#if 0
    assert(m_androidMediaPlayer != NULL);

    assert(m_setVideoSizeCb != NULL);
    android_media_set_video_size_cb(m_androidMediaPlayer, m_setVideoSizeCb, m_setVideoSizeContext);
#endif

    //m_videoOutput->setupSurface();
#if 0
    // Gets called when there is any type of media playback issue
    android_media_set_error_cb(m_androidMediaPlayer, error_msg_cb, static_cast<void *>(this));
#endif
}

void AalMediaPlayerService::createVideoSink(uint32_t texture_id)
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot create a video sink without a valid media-hub player session";
        return;
    }

    // Make the offscreen surface thread context be the current one
    //makeCurrent();

    m_hubPlayerSession->create_video_sink(texture_id);
    // This call will make sure the GLConsumerWrapperHybris gets set on qtvideo-node
    m_videoOutput->updateVideoTexture();

    m_hubPlayerSession->set_frame_available_callback(&AalMediaPlayerService::onFrameAvailableCb, static_cast<void*>(this));
    m_videoOutputReady = true;
}

void AalMediaPlayerService::handleFocusWindowChanged(QWindow *window)
{
    qDebug() << "Handle focusWindowChanged!!";

#if 0
    // Make the offscreen surface thread context be the current one
    makeCurrent(window);

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    qDebug() << "texture_id: " << texture_id;

    m_hubPlayerSession->create_video_sink(texture_id);
    m_videoOutputReady = true;
#endif
}


void AalMediaPlayerService::setMedia(const QUrl &url)
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot open uri without a valid media-hub player session";
        return;
    }
    if (url.isEmpty())
    {
        qWarning() << "Failed to set media source, url must be set." << endl;
        return;
    }

    qDebug() << "Setting media to: " << url;
    const media::Track::UriType uri(url.url().toStdString());
    try {
        m_hubPlayerSession->open_uri(uri);
    }
    catch (std::runtime_error &e) {
        qWarning() << "Failed to open media " << url << ": " << e.what();
        return;
    }

    m_videoOutput->setupSurface();
}

void AalMediaPlayerService::play()
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot start playback without a valid media-hub player session";
        return;
    }

    if (m_videoOutputReady)
    {
        try {
            qDebug() << "Actually calling m_hubPlayerSession->play()";
            m_hubPlayerSession->play();
        }
        catch (std::runtime_error &e) {
            qWarning() << "Failed to start playback: " << e.what();
            return;
        }
    }
    else
        Q_EMIT serviceReady();
}

void AalMediaPlayerService::pause()
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot pause playback without a valid media-hub player session";
        return;
    }

    try {
        m_hubPlayerSession->pause();
    }
    catch (std::runtime_error &e) {
        qWarning() << "Failed to pause playback: " << e.what();
        return;
    }
}

void AalMediaPlayerService::stop()
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot stop playback without a valid media-hub player session";
        return;
    }

    try {
        m_hubPlayerSession->stop();
    }
    catch (std::runtime_error &e) {
        qWarning() << "Failed to stop playback: " << e.what();
        return;
    }
}

int AalMediaPlayerService::position() const
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot get current playback position without a valid media-hub player session";
        return 0;
    }

    try {
        return m_hubPlayerSession->position() * 1e-6;
    }
    catch (std::runtime_error &e) {
        qWarning() << "Failed to get current playback position: " << e.what();
        return 0;
    }
}

void AalMediaPlayerService::setPosition(int msec)
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot set current playback position without a valid media-hub player session";
        return;
    }
    m_hubPlayerSession->seek_to(std::chrono::microseconds{msec * 1000});
}

int AalMediaPlayerService::duration() const
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot get playback duration without a valid media-hub player session";
        return 0;
    }

    try {
        return m_hubPlayerSession->duration() * 1e-6;
    }
    catch (std::runtime_error &e) {
        qWarning() << "Failed to get current playback duration: " << e.what();
        return 0;
    }
}

int AalMediaPlayerService::getVolume() const
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot get volume without a valid media-hub player session";
        return 0;
    }

    try {
        return m_hubPlayerSession->volume();
    }
    catch (std::runtime_error &e) {
        qWarning() << "Failed to get current volume level: " << e.what();
        return 0;
    }
}

void AalMediaPlayerService::setVolume(int volume)
{
    if (m_hubPlayerSession == NULL)
    {
        qWarning() << "Cannot set volume without a valid media-hub player session";
        return;
    }

#if 0
    assert(m_androidMediaPlayer != NULL);

    int ret = android_media_set_volume(m_androidMediaPlayer, volume);
    if (ret != OK)
    {
        qWarning() << "Failed to set the volume." << endl;
    }
#endif
}

void AalMediaPlayerService::onFrameAvailableCb(void *context)
{
    if (context != NULL)
    {
        qDebug() << "Calling onFrameAvailable()";
        AalMediaPlayerService *s = static_cast<AalMediaPlayerService*>(context);
        s->onFrameAvailable();
    }
}

void AalMediaPlayerService::onFrameAvailable()
{
    qDebug() << "Calling m_videoOutput->updateVideoTexture()";
    m_videoOutput->updateVideoTexture();
}

QWindow *AalMediaPlayerService::createOffscreenWindow(const QSurfaceFormat &format)
{
    // Create a non-visible window for getting the proper renderer thread context
    // and surfaceFormat
    QWindow *w = new QWindow();
    w->setSurfaceType(QWindow::OpenGLSurface);
    w->setFormat(format);
    w->setGeometry(0, 0, 1, 1);
    w->setFlags(w->flags() | Qt::WindowTransparentForInput);
    w->create();

    return w;
}

void AalMediaPlayerService::makeCurrent(QWindow *window)
{
    qDebug() << Q_FUNC_INFO;

    QOpenGLContext *currContext = QOpenGLContext::currentContext();

    // Get the currently focused window, which should be the app's window
    //QWindow *window = QGuiApplication::focusWindow();
    QQuickWindow *w = dynamic_cast<QQuickWindow*>(window);
    // If we don't have a GL context in the current thread, create one and share it
    // with the render thread GL context
    if (!currContext && !m_glContext) {
        // This emulates the QOffscreenWindow class that comes with Qt 5.1+
        m_offscreenSurface = createOffscreenWindow(w->openglContext()->surface()->format());
        m_offscreenSurface->setParent(window);

        QOpenGLContext *shareContext = 0;
        if (m_videoOutput->surface())
            shareContext = qobject_cast<QOpenGLContext*>(m_videoOutput->surface()->property("GLContext").value<QObject*>());

        m_glContext = new QOpenGLContext();
        m_glContext->setFormat(m_offscreenSurface->requestedFormat());

        if (shareContext)
            m_glContext->setShareContext(shareContext);

        if (!m_glContext->create()) {
            qWarning() << Q_FUNC_INFO << ": Failed to create new shared context";
            return;
        }
    }

    if (m_glContext)
        m_glContext->makeCurrent(m_offscreenSurface);
}

void AalMediaPlayerService::setVideoTextureNeedsUpdateCb(on_video_texture_needs_update cb, void *context)
{
#if 0
    assert(m_androidMediaPlayer != NULL);

    android_media_set_video_texture_needs_update_cb(m_androidMediaPlayer, cb, context);
#endif
}

void AalMediaPlayerService::setVideoSizeCb(on_msg_set_video_size cb, void *context)
{
    m_setVideoSizeCb = cb;
    m_setVideoSizeContext = context;
}

void AalMediaPlayerService::setPlaybackCompleteCb(on_playback_complete cb, void *context)
{
#if 0
    assert(m_androidMediaPlayer != NULL);

    android_media_set_playback_complete_cb(m_androidMediaPlayer, cb, context);
#endif
}

void AalMediaPlayerService::setMediaPreparedCb(on_media_prepared cb, void *context)
{
#if 0
    assert(m_androidMediaPlayer != NULL);

    android_media_set_media_prepared_cb(m_androidMediaPlayer, cb, context);
#endif
}
