#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>

struct Track
{
	std::string artist;
	std::string title;
	std::string url;
};

struct Playlist
{	
	std::string id;
	std::string title;
	std::string owner_id;
	std::string access_key;
};

std::vector<Track> parse_music_list(const std::string& json_raw);
std::vector<Playlist> parse_playlist(const std::string& json_raw);
#endif