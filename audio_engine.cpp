#include "audio_engine.hpp"
#include <iostream>

AudioPlayer::AudioPlayer() {
	handle = mpv_create();
	if (!handle) throw std::runtime_error("\nfailed create mpv handle");
	mpv_set_property_string(handle, "vo", "null");
	mpv_initialize(handle);
	mpv_request_event(handle, MPV_EVENT_END_FILE, 1);
}

AudioPlayer::~AudioPlayer() {
	running = false;
	if (event_thread.joinable()) event_thread.join();
	mpv_terminate_destroy(handle);
}

void AudioPlayer::play(const std::string& url) {
	const char *cmd[] = {"loadfile", url.c_str(), NULL};
	mpv_command(handle, cmd);
	int pause = 0;
	mpv_set_property(handle, "pause", MPV_FORMAT_FLAG, &pause);
    std::cout << "track started\n";
}

void AudioPlayer::start_event_loop() {
	event_thread = std::thread([this]() {
		while (running) {
			mpv_event *event = mpv_wait_event(handle, 1000);

			if (!event || event->event_id == MPV_EVENT_NONE) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			if (event->event_id == MPV_EVENT_SHUTDOWN) break;

			if (event->event_id == MPV_EVENT_END_FILE) {
				// mpv_event_end_file *end = (mpv_event_end_file *)event->data;
				auto end = static_cast<mpv_event_end_file *>(event->data);
				if (end && end->reason == MPV_END_FILE_REASON_EOF) track_finished = true;
			}
		}
	});
}

bool AudioPlayer::is_track_finished() {
	if (track_finished) {
		track_finished = false;
		return true;
	}
	return false;
}

void AudioPlayer::toggle_pause() {
	int pause = 0;
	mpv_get_property(handle, "pause", MPV_FORMAT_FLAG, &pause);
	pause = !pause;
	mpv_set_property(handle, "pause", MPV_FORMAT_FLAG, &pause);
}

void AudioPlayer::stop() {
	const char *cmd[] = {"stop", NULL};
	mpv_command(handle, cmd);
	mpv_wakeup(handle);
}
