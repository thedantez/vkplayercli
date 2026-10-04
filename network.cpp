#include "network.hpp"

//callback for curl: wrote data
size_t write_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string send_request(const std::string& method, const std::string& params, const std::string& token, int offset) {
	CURL* curl = curl_easy_init();
	std::string read_buffer;

	if (curl) {//if api.vk.com not working then use api.vk.ru
		std::string url = "https://api.vk.ru/method/" + method + "?" + params + "&offset=" + std::to_string(offset) + "&access_token=" + token + "&v=5.199";

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "KateMobileAndroid/56 lite (Android 4.4.2; SDK 19; x86; google Nexus 5; en)");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return read_buffer;
}

int total_count_all_tracks(const std::string& token) {
	std::string json = send_request("audio.get", "count=0&https=1&owner_id=587259366", token);

	try {
		auto j = nlohmann::json::parse(json);
		if (j.contains("response") && j["response"].contains("count")) {
			return (int)j["response"]["count"]; //total_count
		}
	} catch (const nlohmann::json::parse_error& e) {
		std::cerr << "JSON error: " << e.what() << '\n';
	}
	return 0;
}
