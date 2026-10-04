#ifndef UI_HPP
#define UI_HPP

#include "parser.hpp"
#include "audio_engine.hpp"
#include "network.hpp"
#include <vector>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <algorithm>
#include <random>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <poll.h>
#include <fstream>

enum class ViewState {
	TRACKS, //mode viewing tracks
	PLAYLISTS, //mode viewing playlists
	VIEWPLAYLIST //mode viewing single playlist
};

class PlayerUI {
public:
	void run(const std::vector<Track>& playlist, const std::string& token);
private:
	void clear_console();
	void draw_tracks(const std::vector<Track>& playlist, int current_idx);
	void draw_playlists(const std::vector<Playlist>& playlists, int current_idx);
	void draw_header(const std::string& title);
	std::vector<Track> get_all_tracks(const std::string& token);
	void save_playlist(const std::vector<Track> playlist);
	char get_key(); //reading keys
	ViewState current_view = ViewState::TRACKS;
	std::string current_playlist_title = "ALL TRACKS";
};

#endif
