#pragma once
#include <json/json.h>
#include <functional>
#include <string>

// Interface: defines the contract that every weather provider must follow.
// No implementation here, only method signatures.
class IWeatherProvider {
public:
    virtual ~IWeatherProvider() = default;

    // Retrieves the current weather for given coordinates.
    // callback(success, jsonResult) is called asynchronously.
    virtual void fetchCurrentWeather(
        const std::string& lat,
        const std::string& lon,
        std::function<void(bool success, const Json::Value& result)> callback) = 0;

    // Checks if the external service is reachable (used by /health/ready)
    virtual void checkAvailability(
        std::function<void(bool available)> callback) = 0;
};
