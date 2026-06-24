#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(int argc, char* argv[]) {
    // Coordinates for Rennes (Latitude: 48.11, Longitude: -1.67)
    std::string lat = "48.11";
    std::string lon = "-1.67";

    if (argc >= 3) {
        lat = argv[1];
        lon = argv[2];
    }

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true";
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            std::cerr << "HTTP error" << std::endl;
            return 1;
        }

        try {
            auto j = json::parse(readBuffer);
            std::cout << j["current_weather"].dump() << std::endl; // Output raw JSON for FastAPI
        } 
        catch (json::exception& e) {
            return 1;
        }
    }
    return 0;
}