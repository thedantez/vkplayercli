#ifndef AUDIO_ENGINE_HPP
#define AUDIO_ENGINE_HPP

#include <string>
#include <mpv/client.h>
#include <stdexcept>
#include <atomic>
#include <thread>
#include <chrono>

class AudioPlayer {
public:
	AudioPlayer();
	~AudioPlayer();
	void play(const std::string& url);
	void toggle_pause();
	void stop();
	void start_event_loop();
	bool is_track_finished();
private:
	mpv_handle *handle;
	std::atomic<bool> track_finished{false};
	std::atomic<bool> running{true};
	std::thread event_thread;
};

#endif