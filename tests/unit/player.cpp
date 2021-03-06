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

#include "player.h"

#include <QtGlobal>
#include <QtTest/QtTest>

using namespace std;

struct core::ubuntu::media::TestPlayer::Configuration
{
};

namespace core {
namespace ubuntu {
namespace media {

typedef void* GLConsumerWrapperHybris;

const TestPlayer::Configuration& TestPlayer::Client::default_configuration()
{
    static const TestPlayer::Configuration config;
    return config;
}

Player::Player()
{
}

Player::~Player()
{
}

TestPlayer::TestPlayer()
{
}

TestPlayer::~TestPlayer()
{
}

std::string TestPlayer::uuid() const
{
    return std::string{};
}

void TestPlayer::reconnect()
{
}

void TestPlayer::abandon()
{
}

shared_ptr<TrackList> TestPlayer::track_list()
{
    static shared_ptr<TrackList> ret(NULL);
    return ret;
}

Player::PlayerKey TestPlayer::key() const
{
    return 0;
}

media::video::Sink::Ptr TestPlayer::create_gl_texture_video_sink(uint32_t texture_id)
{
    Q_UNUSED(texture_id);
    throw std::runtime_error{"Not implemented"};
}

bool TestPlayer::open_uri(const Track::UriType& uri)
{
    Q_UNUSED(uri);
    return true;
}

bool TestPlayer::open_uri(const Track::UriType& uri, const HeadersType&)
{
    Q_UNUSED(uri);
    return true;
}

void TestPlayer::next()
{
}

void TestPlayer::previous()
{
}

void TestPlayer::play()
{
}

void TestPlayer::pause()
{
}

void TestPlayer::stop()
{
}

void TestPlayer::seek_to(const std::chrono::microseconds& offset)
{
    m_position.set(offset.count());
}

const core::Property<bool>& TestPlayer::can_play() const
{
    static core::Property<bool> ret(true);
    return ret;
}

const core::Property<bool>& TestPlayer::can_pause() const
{
    static core::Property<bool> ret(true);
    return ret;
}

const core::Property<bool>& TestPlayer::can_seek() const
{
    static core::Property<bool> ret(true);
    return ret;
}

const core::Property<bool>& TestPlayer::can_go_previous() const
{
    static core::Property<bool> ret(true);
    return ret;
}

const core::Property<bool>& TestPlayer::can_go_next() const
{
    static core::Property<bool> ret(true);
    return ret;
}

const core::Property<bool>& TestPlayer::is_video_source() const
{
    static core::Property<bool> ret(true);
    return ret;
}

const core::Property<bool>& TestPlayer::is_audio_source() const
{
    static core::Property<bool> ret(true);
    return ret;
}

const core::Property<TestPlayer::PlaybackStatus>& TestPlayer::playback_status() const
{
    static core::Property<Player::PlaybackStatus> ret(Player::PlaybackStatus::null);
    return ret;
}

const core::Property<AVBackend::Backend>& TestPlayer::backend() const
{
    static core::Property<AVBackend::Backend> ret(AVBackend::Backend::none);
    return ret;
}

const core::Property<Player::LoopStatus>& TestPlayer::loop_status() const
{
    static core::Property<Player::LoopStatus> ret(Player::LoopStatus::none);
    return ret;
}

const core::Property<Player::PlaybackRate>& TestPlayer::playback_rate() const
{
    static core::Property<Player::PlaybackRate> ret(1);
    return ret;
}

const core::Property<bool>& TestPlayer::shuffle() const
{
    static core::Property<bool> ret(true);
    return ret;
}

const core::Property<Track::MetaData>& TestPlayer::meta_data_for_current_track() const
{
    static core::Property<Track::MetaData> ret;
    return ret;
}

const core::Property<Player::Volume>& TestPlayer::volume() const
{
    static core::Property<Volume> ret(1);
    return ret;
}

const core::Property<Player::PlaybackRate>& TestPlayer::minimum_playback_rate() const
{
    static core::Property<Volume> ret(1);
    return ret;
}

const core::Property<Player::PlaybackRate>& TestPlayer::maximum_playback_rate() const
{
    static core::Property<Volume> ret(8);
    return ret;
}

const core::Property<int64_t>& TestPlayer::position() const
{
    return m_position;
}

const core::Property<int64_t>& TestPlayer::duration() const
{
    static const core::Property<int64_t> dur(1e6);
    return dur;
}

const core::Property<Player::AudioStreamRole>& TestPlayer::audio_stream_role() const
{
    static const core::Property<Player::AudioStreamRole> role(Player::AudioStreamRole::multimedia);
    return role;
}

const core::Property<Player::Orientation>& TestPlayer::orientation() const
{
    static const core::Property<Player::Orientation> orientation(Player::Orientation::rotate0);
    return orientation;
}

const core::Property<Player::Lifetime>& TestPlayer::lifetime() const
{
    static const core::Property<Player::Lifetime> lifetime(Player::Lifetime::normal);
    return lifetime;
}

core::Property<Player::LoopStatus>& TestPlayer::loop_status()
{
    static core::Property<Player::LoopStatus> ret(Player::LoopStatus::none);
    return ret;
}

core::Property<Player::PlaybackRate>& TestPlayer::playback_rate()
{
    static core::Property<Player::PlaybackRate> ret(1);
    return ret;
}

core::Property<bool>& TestPlayer::shuffle()
{
    static core::Property<bool> ret(true);
    return ret;
}

core::Property<Player::Volume>& TestPlayer::volume()
{
    static core::Property<Volume> ret(1);
    return ret;
}

core::Property<Player::AudioStreamRole>& TestPlayer::audio_stream_role()
{
    static core::Property<Player::AudioStreamRole> role(Player::AudioStreamRole::multimedia);
    return role;
}

core::Property<Player::Lifetime>& TestPlayer::lifetime()
{
    static core::Property<Player::Lifetime> lifetime(Player::Lifetime::normal);
    return lifetime;
}

const core::Signal<int64_t>& TestPlayer::seeked_to() const
{
    static core::Signal<int64_t> ret;
    return ret;
}

const core::Signal<void>& TestPlayer::about_to_finish() const
{
    static core::Signal<void> ret;
    return ret;
}

const core::Signal<void>& TestPlayer::end_of_stream() const
{
    static core::Signal<void> ret;
    return ret;
}

core::Signal<Player::PlaybackStatus>& TestPlayer::playback_status_changed()
{
    static core::Signal<Player::PlaybackStatus> ret;
    return ret;
}

const core::Signal<core::ubuntu::media::video::Dimensions>& TestPlayer::video_dimension_changed() const
{
    static core::Signal<core::ubuntu::media::video::Dimensions> ret;
    return ret;
}

const core::Signal<Player::Error>& TestPlayer::error() const
{
    static core::Signal<Player::Error> ret;
    return ret;
}

const core::Signal<int>& TestPlayer::buffering_changed() const
{
    static const core::Signal<int> dur;
    return dur;
}

const std::shared_ptr<Service> Service::Client::instance()
{
    return NULL;
}

const Track::Id& TrackList::after_empty_track()
{
    static const Track::Id track_id;
    return track_id;
}
}
}
}
