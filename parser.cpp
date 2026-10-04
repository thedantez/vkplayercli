#include "parser.hpp"

std::vector<Track> parse_music_list(const std::string& json_raw) {
	std::vector<Track> tracks;

	try {
		//parse string to json
		auto j = nlohmann::json::parse(json_raw);
		//check if exist errors
		if (j.contains("error")) {
			std::cerr << "VK API error: " << j["error"]["error_msg"] << "\n";
			return tracks;
		}

		if (j.contains("response") && j["response"].contains("items")) {
			for (const auto& item : j["response"]["items"]) {
				Track t;

				t.artist = item.value("artist", "Unknown artist");
				t.title = item.value("title", "Unknown title");
				t.url = item.value("url", "");

				if (!t.url.empty()) {
					tracks.push_back(t);
				}
			}
		}
	} catch (const nlohmann::json::parse_error& e) {
		std::cerr << "\nJSON parse error: " << e.what() << '\n';
	}

	return tracks;
}

std::vector<Playlist> parse_playlist(const std::string& json_raw) {
	std::vector<Playlist> playlists;
	try {
		auto j = nlohmann::json::parse(json_raw);
		//check for error API
		if (j.contains("error")) {
			std::cerr << "VK API error: " << j["error"]["error_msg"] << '\n';
			return playlists;
		}
		//parsing items
		if (j.contains("response") && j["response"].contains("items")) {
			for (const auto& item : j["response"]["items"]) {
				Playlist p;

				p.id = std::to_string(item.value("id", 0));
				p.title = item.value("title", "Unknown title");
				p.owner_id = std::to_string(item.value("owner_id", 0));
				p.access_key = item.value("access_key", "");

				if (p.id != "0") {
					playlists.push_back(p);
				}
			}
		}
	} catch (const nlohmann::json::parse_error& e) {
		std::cerr << "\nJSON parse error: " << e.what() << '\n';
	}

	return playlists;
}

