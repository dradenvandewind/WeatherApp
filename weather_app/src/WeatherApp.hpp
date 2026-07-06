#pragma once
#include <drogon/drogon.h>
#include <string>
#include <functional>

class WeatherApp {
public:
    WeatherApp();
    ~WeatherApp();

    WeatherApp(const WeatherApp&) = delete;
    WeatherApp& operator=(const WeatherApp&) = delete;

    void initAndRun(const std::string& host = "0.0.0.0", uint16_t port = 8000, size_t threads = 16);

private:
    void registerRoutes();

    // Handlers
    void handleOpenApi(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handleDocs(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handleLiveness(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handleReadiness(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handleTests(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handleWeather(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    // static data / schemas
    static const std::string openApiSchema_;
    static const std::string swaggerHtml_;
};