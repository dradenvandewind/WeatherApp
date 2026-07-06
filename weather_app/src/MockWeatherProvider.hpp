#pragma once
#include "IWeatherProvider.hpp"

// Fake implementation used in /test: makes no network calls,
// returns fixed data to keep tests fast and deterministic.
class MockWeatherProvider : public IWeatherProvider {
public:
    void fetchCurrentWeather(
        const std::string& lat,
        const std::string& lon,
        std::function<void(bool success, const Json::Value& result)> callback) override
    {
        Json::Value current;
        current["temperature"] = 18.5;
        current["windspeed"] = 12.3;
        current["weathercode"] = 1;
        current["time"] = "2024-01-15T14:00";

        Json::Value root;
        root["current_weather"] = current;
        root["_mock_lat"] = lat;
        root["_mock_lon"] = lon;

        callback(true, root);
    }

    void checkAvailability(std::function<void(bool available)> callback) override
    {
        callback(true);
    }
};
