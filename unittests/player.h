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

#ifndef PLAYER_H_
#define PLAYER_H_

#include <core/media/service.h>
#include <core/media/player.h>
#include <core/media/track_list.h>

#include <cstdint>
#include <memory>

#include <QtTest/QtTest>

namespace core {
namespace ubuntu {
namespace media {

class TestPlayer : public Player
{
public:
    TestPlayer();
    virtual ~TestPlayer();

    virtual std::shared_ptr<TrackList> track_list();
    virtual PlayerKey key() const;

    virtual bool open_uri(const Track::UriType& uri);
    virtual bool open_uri(const Track::UriType&, const HeadersType&);
    virtual void create_video_sink(uint32_t texture_id);
    virtual GLConsumerWrapperHybris gl_consumer() const;
    virtual void next();
    virtual void previous();
    virtual void play();
    virtual void pause();
    virtual void seek_to(const std::chrono::microseconds& offset);
    virtual void stop();

    virtual void set_frame_available_callback(FrameAvailableCb cb, void *context);
    virtual void set_playback_complete_callback(PlaybackCompleteCb cb, void *context);

    virtual const core::Property<bool>& can_play() const;
    virtual const core::Property<bool>& can_pause() const;
    virtual const core::Property<bool>& can_seek() const;
    virtual const core::Property<bool>& can_go_previous() const;
    virtual const core::Property<bool>& can_go_next() const;
    virtual const core::Property<bool>& is_video_source() const;
    virtual const core::Property<bool>& is_audio_source() const;
    virtual const core::Property<PlaybackStatus>& playback_status() const;
    virtual const core::Property<Orientation>& orientation() const;
    virtual const core::Property<LoopStatus>& loop_status() const;
    virtual const core::Property<PlaybackRate>& playback_rate() const;
    virtual const core::Property<bool>& is_shuffle() const;
    virtual const core::Property<Track::MetaData>& meta_data_for_current_track() const;
    virtual const core::Property<Volume>& volume() const;
    virtual const core::Property<PlaybackRate>& minimum_playback_rate() const;
    virtual const core::Property<PlaybackRate>& maximum_playback_rate() const;
    virtual const core::Property<int64_t>& position() const;
    virtual const core::Property<int64_t>& duration() const;
    virtual const core::Property<AudioStreamRole>& audio_stream_role() const;
    virtual const core::Property<Lifetime>& lifetime() const;

    virtual core::Property<LoopStatus>& loop_status();
    virtual core::Property<PlaybackRate>& playback_rate();
    virtual core::Property<bool>& is_shuffle();
    virtual core::Property<Volume>& volume();
    virtual core::Property<AudioStreamRole>& audio_stream_role();
    virtual core::Property<Lifetime>& lifetime();

    virtual const core::Signal<int64_t>& seeked_to() const;
    virtual const core::Signal<void>& end_of_stream() const;
    virtual const core::Signal<PlaybackStatus>& playback_status_changed() const;
    virtual core::Signal<PlaybackStatus>& playback_status_changed();
    virtual const core::Signal<uint64_t>& video_dimension_changed() const;
    virtual const core::Signal<Error>& error() const;

private:
    core::Property<int64_t> m_position;
};

}
}
}

#endif
