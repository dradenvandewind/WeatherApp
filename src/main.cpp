#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Callback to store the libcurl response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        // Coordinates for Rennes (Latitude: 48.11, Longitude: -1.67)
        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=48.11&longitude=-1.67&current_weather=true";
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // Execute the request
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return 1;
        }

        try {
            // Parse the JSON
            auto j = json::parse(readBuffer);
            
            double temperature = j["current_weather"]["temperature"];
            double windspeed = j["current_weather"]["windspeed"];
            
            std::cout << "=== Real-time weather (Rennes) ===" << std::endl;
            std::cout << "Temperature: " << temperature << "°C" << std::endl;
            std::cout << "Wind speed: " << windspeed << " km/h" << std::endl;
        } 
        catch (json::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            return 1;
        }
    }
    return 0;
}
