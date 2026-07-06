#pragma once
#include "IWeatherProvider.hpp"
#include <drogon/drogon.h>
#include <memory>
#include <string>
#include <functional>

class WeatherApp {
public:
    // Injection de dependance : WeatherApp recoit un IWeatherProvider
    // (OpenMeteoProvider en production, MockWeatherProvider en test).
    explicit WeatherApp(std::unique_ptr<IWeatherProvider> provider);
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

    // Le fournisseur meteo n'est plus fige en dur : c'est une interface.
    std::unique_ptr<IWeatherProvider> weatherProvider_;

    // static data / schemas
    static const std::string openApiSchema_;
    static const std::string swaggerHtml_;
};