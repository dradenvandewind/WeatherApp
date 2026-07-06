#pragma once
#include "IWeatherProvider.hpp"
#include <drogon/drogon.h>

// Concrete implementation: actually talks to the Open-Meteo service over HTTP.
class OpenMeteoProvider : public IWeatherProvider {
public:
    void fetchCurrentWeather(
        const std::string& lat,
        const std::string& lon,
        std::function<void(bool success, const Json::Value& result)> callback) override
    {
        auto client = drogon::HttpClient::newHttpClient("https://api.open-meteo.com");
        std::string path = "/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true";
        auto apiReq = drogon::HttpRequest::newHttpRequest();
        apiReq->setPath(path);

        client->sendRequest(apiReq, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
            if (result != drogon::ReqResult::Ok || !response) {
                callback(false, Json::Value());
                return;
            }
            auto jsonPtr = response->getJsonObject();
            if (!jsonPtr) {
                callback(false, Json::Value());
                return;
            }
            callback(true, *jsonPtr);
        });
    }

    void checkAvailability(std::function<void(bool available)> callback) override
    {
        auto client = drogon::HttpClient::newHttpClient("https://api.open-meteo.com");
        auto probe = drogon::HttpRequest::newHttpRequest();
        probe->setPath("/v1/forecast?latitude=48.8566&longitude=2.3522&current_weather=true");

        client->sendRequest(probe, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
            bool ok = (result == drogon::ReqResult::Ok && response && response->getStatusCode() == drogon::k200OK);
            callback(ok);
        });
    }
};
