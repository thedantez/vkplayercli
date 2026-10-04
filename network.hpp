#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "parser.hpp"
#include <string>
#include <curl/curl.h>

std::string send_request(const std::string& method, const std::string& params, const std::string& token, int offset = 0);
int total_count_all_tracks(const std::string& token);

#endif