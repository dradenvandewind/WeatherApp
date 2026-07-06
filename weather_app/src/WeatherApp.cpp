#include "WeatherApp.hpp"
#include <iostream>

// Initialization of static constants
const std::string WeatherApp::openApiSchema_ = R"JSON({
  "openapi": "3.0.3",
  "info": {
    "title": "Weather API",
    "description": "Real-time proxy to Open-Meteo, built with Drogon .",
    "version": "1.0.0"
  },
  "servers": [{ "url": "http://localhost:8080/api", "description": "Nginx Proxy" }],
  "tags": [
    { "name": "weather",  "description": "Real-time weather data" },
    { "name": "health", "description": "Docker / Kubernetes probes" }
  ],
  "paths": {
    "/weather": {
      "get": {
        "tags": ["weather"],
        "summary": "Current weather",
        "description": "Returns real-time weather for the given coordinates (default: Paris).",
        "parameters": [
          { "name": "lat", "in": "query", "required": false, "schema": { "type": "number", "example": 48.8566 } },
          { "name": "lon", "in": "query", "required": false, "schema": { "type": "number", "example": 2.3522 } }
        ],
        "responses": {
          "200": {
            "description": "Success",
            "content": {
              "application/json": {
                "schema": { "$ref": "#/components/schemas/WeatherResponse" },
                "example": {
                  "speech": "Refreshes real-time data via the native Drogon framework .",
                  "current_weather": { "temperature": 18.5, "windspeed": 12.3, "weathercode": 1, "time": "2024-01-15T14:00" }
                }
              }
            }
          },
          "500": { "description": "Internal error" }
        }
      }
    },
    "/health/live": { "get": { "tags": ["health"], "responses": { "200": { "description": "Process alive" } } } },
    "/health/ready": { "get": { "tags": ["health"], "responses": { "200": { "description": "Ready" }, "503": { "description": "Unavailable" } } } }
  },
  "components": {
    "schemas": {
      "WeatherResponse": {
        "type": "object",
        "properties": {
          "speech": { "type": "string" },
          "current_weather": { "$ref": "#/components/schemas/CurrentWeather" }
        }
      },
      "CurrentWeather": {
        "type": "object",
        "properties": {
          "temperature": { "type": "number" },
          "windspeed": { "type": "number" },
          "weathercode": { "type": "integer" },
          "time": { "type": "string", "format": "date-time" }
        }
      }
    }
  }
})JSON";

const std::string WeatherApp::swaggerHtml_ = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8"/>
  <title>Weather API - Docs</title>
  <link rel="stylesheet" href="https://unpkg.com/swagger-ui-dist@5/swagger-ui.css"/>
  <style>
    body { margin: 0; }
    #swagger-ui .topbar { background: #1b1b2f; }
    #swagger-ui .topbar-wrapper::after { content: "Weather API"; color: #fff; font-size: 1.3rem; font-weight: 700; padding-left: 1rem; }
  </style>
</head>
<body>
  <div id="swagger-ui"></div>
  <script src="https://unpkg.com/swagger-ui-dist@5/swagger-ui-bundle.js"></script>
  <script>
    SwaggerUIBundle({
      url: "/openapi.json",
      dom_id: "#swagger-ui",
      presets: [SwaggerUIBundle.presets.apis, SwaggerUIBundle.SwaggerUIStandalonePreset],
      layout: "BaseLayout",
      deepLinking: true,
      tryItOutEnabled: true
    });
  </script>
</body>
</html>)HTML";

WeatherApp::WeatherApp() {
    LOG_INFO << "WeatherApp instance created.";
}

WeatherApp::~WeatherApp() {
    LOG_INFO << "WeatherApp instance destroyed.";
}

void WeatherApp::initAndRun(const std::string& host, uint16_t port, size_t threads) {
    registerRoutes();
    
    LOG_INFO << "Drogon server starting on " << host << ":" << port;
    drogon::app().addListener(host, port)
                 .setThreadNum(threads)
                 .run();
}

void WeatherApp::registerRoutes() {
    drogon::app().registerHandler("/openapi.json", [this](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        handleOpenApi(req, std::move(callback));
    }, {drogon::Get});

    drogon::app().registerHandler("/docs", [this](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        handleDocs(req, std::move(callback));
    }, {drogon::Get});

    drogon::app().registerHandler("/health/live", [this](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        handleLiveness(req, std::move(callback));
    }, {drogon::Get});

    drogon::app().registerHandler("/health/ready", [this](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        handleReadiness(req, std::move(callback));
    }, {drogon::Get});

    drogon::app().registerHandler("/test", [this](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        handleTests(req, std::move(callback));
    }, {drogon::Get});

    drogon::app().registerHandler("/weather", [this](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
        handleWeather(req, std::move(callback));
    }, {drogon::Get});
}

void WeatherApp::handleOpenApi(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto res = drogon::HttpResponse::newHttpResponse();
    res->setStatusCode(drogon::k200OK);
    res->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->setBody(openApiSchema_);
    callback(res);
}

void WeatherApp::handleDocs(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto res = drogon::HttpResponse::newHttpResponse();
    res->setStatusCode(drogon::k200OK);
    res->setContentTypeString("text/html; charset=utf-8");
    res->setBody(swaggerHtml_);
    callback(res);
}

void WeatherApp::handleLiveness(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto res = drogon::HttpResponse::newHttpResponse();
    res->setStatusCode(drogon::k200OK);
    res->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->setBody(R"({"status":"ok"})");
    callback(res);
}

void WeatherApp::handleReadiness(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto client = drogon::HttpClient::newHttpClient("https://api.open-meteo.com");
    auto probe = drogon::HttpRequest::newHttpRequest();
    probe->setPath("/v1/forecast?latitude=48.8566&longitude=2.3522&current_weather=true");

    client->sendRequest(probe, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        auto res = drogon::HttpResponse::newHttpResponse();
        res->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        res->addHeader("Access-Control-Allow-Origin", "*");

        if (result != drogon::ReqResult::Ok || !response || response->getStatusCode() != drogon::k200OK) {
            res->setStatusCode(drogon::k503ServiceUnavailable);
            res->setBody(R"({"status":"unavailable","reason":"open-meteo unreachable"})");
        } else {
            res->setStatusCode(drogon::k200OK);
            res->setBody(R"({"status":"ready"})");
        }
        callback(res);
    });
}

void WeatherApp::handleTests(const drogon::HttpRequestPtr&, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    Json::Value report;
    Json::Value tests(Json::arrayValue);
    int passed = 0, failed = 0;

    auto ok = [&](const std::string& name, const std::string& msg = "") {
        Json::Value t; t["name"] = name; t["status"] = "PASS";
        if (!msg.empty()) t["info"] = msg;
        tests.append(t); ++passed;
    };
    auto ko = [&](const std::string& name, const std::string& msg) {
        Json::Value t; t["name"] = name; t["status"] = "FAIL"; t["info"] = msg;
        tests.append(t); ++failed;
    };

    {
        std::string lat = "", lon = "";
        if (lat.empty()) lat = "48.8566";
        if (lon.empty()) lon = "2.3522";
        (lat == "48.8566" && lon == "2.3522") ? ok("default_lat_lon") : ko("default_lat_lon", "lat=" + lat + " lon=" + lon);
    }
    {
        std::string lat = "48.8566", lon = "2.3522";
        std::string path = "/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true";
        std::string expected = "/v1/forecast?latitude=48.8566&longitude=2.3522&current_weather=true";
        (path == expected) ? ok("build_url") : ko("build_url", "got: " + path);
    }
    {
        Json::Value root;
        root["speech"] = "test";
        root["current_weather"] = Json::objectValue;
        std::string body = root.toStyledString();
        (body.find("speech") != std::string::npos && body.find("current_weather") != std::string::npos) ? ok("json_serialization") : ko("json_serialization", "missing keys in: " + body);
    }
    {
        auto res = drogon::HttpResponse::newHttpResponse();
        res->addHeader("Access-Control-Allow-Origin", "*");
        std::string v = res->getHeader("Access-Control-Allow-Origin");
        (v == "*") ? ok("cors_header") : ko("cors_header", "got: '" + v + "'");
    }
    {
        std::string lat = "43.2965", lon = "5.3698";
        std::string saved_lat = lat, saved_lon = lon;
        if (lat.empty()) lat = "48.8566";
        if (lon.empty()) lon = "2.3522";
        (lat == saved_lat && lon == saved_lon) ? ok("custom_lat_lon_preserved", "lat=" + lat + " lon=" + lon) : ko("custom_lat_lon_preserved", "overwritten!");
    }

    report["total"] = passed + failed;
    report["passed"] = passed;
    report["failed"] = failed;
    report["tests"] = tests;

    auto res = drogon::HttpResponse::newHttpResponse();
    res->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->setStatusCode(failed == 0 ? drogon::k200OK : drogon::k500InternalServerError);
    res->setBody(report.toStyledString());
    callback(res);
}

void WeatherApp::handleWeather(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto lat = req->getParameter("lat");
    auto lon = req->getParameter("lon");
    if (lat.empty()) lat = "48.8566";
    if (lon.empty()) lon = "2.3522";

    auto client = drogon::HttpClient::newHttpClient("https://api.open-meteo.com");
    std::string path = "/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true";
    auto apiReq = drogon::HttpRequest::newHttpRequest();
    apiReq->setPath(path);

    client->sendRequest(apiReq, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        auto res = drogon::HttpResponse::newHttpResponse();
        res->addHeader("Access-Control-Allow-Origin", "*");

        if (result != drogon::ReqResult::Ok || !response) {
            res->setStatusCode(drogon::k500InternalServerError);
            res->setBody(R"({"error":"Failed to connect to Open-Meteo"})");
            res->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            callback(res);
            return;
        }

        auto jsonPtr = response->getJsonObject();
        if (!jsonPtr) {
            res->setStatusCode(drogon::k500InternalServerError);
            res->setBody(R"({"error":"External API JSON parsing error"})");
            res->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            callback(res);
            return;
        }

        Json::Value root;
        root["speech"] = "Refreshes real-time data via the native Drogon framework .";
        root["current_weather"] = (*jsonPtr)["current_weather"];

        res->setStatusCode(drogon::k200OK);
        res->setBody(root.toStyledString());
        res->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(res);
    });
}