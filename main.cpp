#include "network.hpp" //take text from vk
#include "parser.hpp" //do json to Track
#include "audio_engine.hpp" //start music play and manipulate it
#include "ui.hpp"

#include <fstream>
#include <filesystem>
#include <regex>

std::string get_token_from_config() {
    std::string config_path = std::string(getenv("HOME")) + "/.config/cmu/config.toml";
    std::ifstream file(config_path);

    if (!file.is_open()) {
        std::cerr << "error w/ openin config file path: " << config_path << "\n";
        return "";
    }

    std::string line;
    std::regex token_regex(R"(token\s*=\s*"([^\"]+)\")");
    std::smatch match;

    while (std::getline(file, line)) {
        if (std::regex_search(line, match, token_regex)) {
            return match[1].str();
        }
    }
    return "";
}

int main() {
    std::string my_token = get_token_from_config();
    if (my_token.empty()) {
        std::cerr << "token not found(\n";
        return 1;
    }
    std::string my_id = "587259366";
	std::string music_raw = send_request("audio.get", "count=15&https=1", my_token);
    std::vector<Track> playlist = parse_music_list(music_raw);
    // std::cout << send_request("audio.getPlaylists", "owner_id=587259366&count=10&https=1", my_token); //debug

    if (!playlist.empty()) {
        PlayerUI ui;
        ui.run(playlist, my_token);
    }

	return 0;
}
