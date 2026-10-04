#include "ui.hpp"

void set_nonblocking_mode() {
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void restore_terminal_mode() {
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
	
	int flags = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}

char PlayerUI::get_key() {
	// char buf = 0;
	// ssize_t n = read(STDIN_FILENO, &buf, 1);
	// if (n > 0) return buf;
	// return 0;
	//new version

	struct pollfd fds;
	fds.fd = STDIN_FILENO;
	fds.events = POLLIN;
	int ret = poll(&fds, 1, 50);

	if (ret > 0 && (fds.revents & POLLIN)) {
		char buf = 0;
		if (read(STDIN_FILENO, &buf, 1) > 0) return buf;
	}
	return 0;
}

void PlayerUI::clear_console() {
	#ifdef _WIN32
		system("cls");
	#else
	 	system("clear");
	#endif
	// std::cout << "\033[2J\033[1;1H";
}

void PlayerUI::draw_header(const std::string& title) {
	int len = title.length();
	std::string line(len + 4, '-');

	std::cout << "\n\t" << line << "\n";
	std::cout << "\t| " << title << " |\n";
	std::cout << "\t" << line << "\n\n";
}

void PlayerUI::draw_tracks(const std::vector<Track>& playlist, int current_idx) {
	clear_console();
	std::cout << "\n---------------------------------\n| VK TUI PLAYER [Thinkpad T480] |\n---------------------------------\n";
	draw_header(current_playlist_title);
	int start = std::max(0, current_idx - 5);
	int end = std::min((int)playlist.size(), start + 15);

	for (int i = start; i < end; ++i) {
		if (i == current_idx) std::cout << "  > \033[1;37m";
		else std::cout << "    ";

		std::cout << playlist[i].artist << " - " << playlist[i].title << "\033[0m\n";
	}
	std::cout << "\n[j/k]: nav | [enter]: playing | [space]: pause | [\\]: shuffle | [p]: >playlist | [q]: quit | [a] all tracks | [s] save tracks\n";
	std::cout << "\nloaded: " << playlist.size() << " tracks\n";
	std::cout << "\n[debug] idx: " << current_idx << " | total: " << playlist.size() << '\n';
}

void PlayerUI::draw_playlists(const std::vector<Playlist>& playlists, int current_idx) {
	clear_console();
	std::cout << "\n---------------------------------\n| VK TUI PLAYER [Thinkpad T480] |\n---------------------------------\n";
	std::cout << "\n\t-------------\n\t| PLAYLISTS |\n\t-------------\n\n";
	int start = std::max(0, current_idx - 5);
	int end = std::min((int)playlists.size(), start + 15);

	for (int i = start; i < end; ++i) {
		if (i == current_idx) std::cout << " > \033[1;37m";
		else std::cout << "    ";

		std::cout << playlists[i].title << "\033[0m\n";
	}
	std::cout << "\n[j/k]: nav | [enter]: playing | [space]: pause | [s]: shuffle | [p]: >playlist | [q]: quit\n";
	std::cout << "\nloaded: " << playlists.size() << " playlists\n";
	std::cout << "\n[debug] idx: " << current_idx << " | total: " << playlists.size() << '\n';	
}

std::vector<Track> PlayerUI::get_all_tracks(const std::string& token) {
	std::vector<Track> all_tracks;
	clear_console();
	int total_count = total_count_all_tracks(token);
	for (int offset = 0; (int)all_tracks.size() < total_count; offset += 100) {
		clear_console();
		std::cout << "\n\tLoading all tracks: " << all_tracks.size() << " / " << total_count << "\n\n";
		auto json = send_request("audio.get", "count=100", token, offset);
		auto chunk = parse_music_list(json);
		if (chunk.empty()) break;
		all_tracks.insert(all_tracks.end(), chunk.begin(), chunk.end());
	}
	clear_console();
	return all_tracks;
}

void PlayerUI::save_playlist(const std::vector<Track> playlist) {
	std::ofstream file("all_tracks.txt"); //opening file

	if (!file.is_open()) {
		clear_console();
		std::cerr << "\n\tError open file" << '\n';
	}

	for (auto i : playlist) { //save tracks
		file << i.artist << '-' << i.title << '\n';
	}

	file.close();
	clear_console();
}

void PlayerUI::run(const std::vector<Track>& initial_playlist, const std::string& token) {
	AudioPlayer player;
    player.start_event_loop();
	//data for tracks
	std::vector<Track> current_playlist = initial_playlist;
	std::vector<Playlist> all_playlists;
	int track_idx = 0, playing_idx = -1, playlist_idx = 0; //cursors
	bool running = true;
	bool need_redraw = true;
	int offset = initial_playlist.size();
	
	std::random_device rd;
	std::mt19937 g(rd());

	//setting terminal in enter
	set_nonblocking_mode();

	struct pollfd fds[1];
	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;

	while (running) {
		if (need_redraw) {
			if (current_view != ViewState::PLAYLISTS) {
				draw_tracks(current_playlist, track_idx);
			} else {
				draw_playlists(all_playlists, playlist_idx);
			}
			need_redraw = false;
		}
		//auto play next track
		if (player.is_track_finished()) {
			track_idx++;
			playing_idx = track_idx;
			if (track_idx >= (int)current_playlist.size()) track_idx = 0;

			player.play(current_playlist[track_idx].url);

			need_redraw = true;
			if (track_idx >= (int)current_playlist.size() - 2) {
				std::string more_ = send_request("audio.get", "count=3&https=1", token, offset);
				std::vector<Track> next_ = parse_music_list(more_);
				if (!next_.empty()) {
					auto start_from = (next_.front().title == current_playlist.back().title && next_.front().artist == current_playlist.back().artist)
						? next_.begin() + 1
						: next_.begin();
					if (start_from != next_.end()) {
						current_playlist.insert(current_playlist.end(), start_from, next_.end());
						offset += next_.size();
					}
				}
			}
		}

		int ret = poll(fds, 1, 100);
		char c = 0;

		if (ret > 0) {
			if (fds[0].revents & POLLIN) {
				c = get_key();
			}
		}

		if (c != 0) {
			need_redraw = true;
		
			switch (c) {
				case 's':
					save_playlist(current_playlist);
					break;
				case 'q':
					restore_terminal_mode();
					player.stop();
					running = false;
					break;
				case 'j':
					if (current_view != ViewState::PLAYLISTS) {
						if (track_idx < (int)current_playlist.size() - 1) {
							track_idx++;

							//update list tracks
							if (track_idx >= (int)current_playlist.size() - 5) {
								std::string more_ = send_request("audio.get", "count=30&https=1", token, offset);
								std::vector<Track> next_ = parse_music_list(more_);

								if (!next_.empty()) {
									auto start_from = (next_.front().title == current_playlist.back().title && next_.front().artist == current_playlist.back().artist)
										? next_.begin() + 1
										: next_.begin();
									if (start_from != next_.end()) {
										current_playlist.insert(current_playlist.end(), start_from, next_.end());
										offset += next_.size();
									}
								}
							}
						}
					} else {
						if (playlist_idx < (int)all_playlists.size() - 1) playlist_idx++;
					}
					break;
				case 'k':
					if (current_view != ViewState::PLAYLISTS) {
						if (track_idx > 0) track_idx--;
					} else {
						if (playlist_idx > 0) playlist_idx--;
					}
					break;
				case 10:
					if (current_view != ViewState::PLAYLISTS) {
						playing_idx = track_idx;
						player.play(current_playlist[track_idx].url);

						std::this_thread::sleep_for(std::chrono::milliseconds(100));

					} else {
						Playlist p = all_playlists[playlist_idx];
						current_playlist_title = p.title;

						std::string params = "album_id=" + p.id + "&owner_id=" + p.owner_id;
						if (!p.access_key.empty()) params += "&access_key=" + p.access_key;
						params += "&count=20&https=1";

						std::string json = send_request("audio.get", params, token);
						std::vector<Track> new_tracks = parse_music_list(json);

						if (!new_tracks.empty()) {
							current_playlist = new_tracks;
							track_idx = 0;
							playing_idx = -1;
							player.stop();
							current_view = ViewState::VIEWPLAYLIST;
						}
					}
					break;
				case 'a':
					restore_terminal_mode();
					current_playlist = get_all_tracks(token);
					set_nonblocking_mode();
					break;
				case ' ':
					player.toggle_pause();
					break;
				case '\\': {
					std::string current_url = "";

					if (playing_idx != -1) {
						current_url = current_playlist[playing_idx].url;
					}

					std::shuffle(current_playlist.begin(), current_playlist.end(), g);
					
					if (!current_url.empty()) {
						for (size_t i = 0; i < current_playlist.size(); ++i) {
							if (current_playlist[i].url == current_url) {
								playing_idx = i;
								break;
							}
						}
					}

					track_idx = 0; //cursor to begin
					// need_redraw = true;
					break;
				}
				case 'p':
					switch (current_view) {
						case ViewState::TRACKS: { //to all playlists
							std::string params = "owner_id=587259366&count=20&https=1";
							std::string json = send_request("audio.getPlaylists", params, token);
							all_playlists = parse_playlist(json);
							current_view = ViewState::PLAYLISTS;
							playlist_idx = 0;
							break;
						}
						case ViewState::PLAYLISTS: //to all tracks
						current_view = ViewState::TRACKS;
						current_playlist_title = "ALL TRACKS";
						break;
						case ViewState::VIEWPLAYLIST: //to all playlists
							current_view = ViewState::PLAYLISTS;
							break;
					}
					break;
			}
		}

		// usleep(100000);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	restore_terminal_mode();
}
