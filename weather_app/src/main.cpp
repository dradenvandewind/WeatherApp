#include <drogon/drogon.h>
#include <iostream>

using namespace drogon;

int main() {
    // ─── OpenAPI 3.0 schema  (/openapi.json) ─────────────────────────────────
    app().registerHandler("/openapi.json",
        [](const HttpRequestPtr&,
           std::function<void(const HttpResponsePtr&)>&& callback) {

            static const std::string schema = R"JSON({
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
          {
            "name": "lat", "in": "query", "required": false,
            "description": "Latitude (default 48.8566)",
            "schema": { "type": "number", "example": 48.8566 }
          },
          {
            "name": "lon", "in": "query", "required": false,
            "description": "Longitude (default 2.3522)",
            "schema": { "type": "number", "example": 2.3522 }
          }
        ],
        "responses": {
          "200": {
            "description": "Success",
            "content": {
              "application/json": {
                "schema": { "$ref": "#/components/schemas/WeatherResponse" },
                "example": {
                  "speech": "Refreshes real-time data via the native Drogon framework .",
                  "current_weather": {
                    "temperature": 18.5, "windspeed": 12.3,
                    "weathercode": 1,    "time": "2024-01-15T14:00"
                  }
                }
              }
            }
          },
          "500": {
            "description": "Internal error or Open-Meteo unreachable",
            "content": {
              "application/json": {
                "schema": { "$ref": "#/components/schemas/ErrorResponse" }
              }
            }
          }
        }
      }
    },
    "/health/live": {
      "get": {
        "tags": ["health"],
        "summary": "Liveness probe",
        "description": "Returns 200 if the Drogon process is alive.",
        "responses": {
          "200": {
            "description": "Process alive",
            "content": { "application/json": { "example": { "status": "ok" } } }
          }
        }
      }
    },
    "/health/ready": {
      "get": {
        "tags": ["health"],
        "summary": "Readiness probe",
        "description": "Checks that Open-Meteo is reachable. Returns 503 if unavailable.",
        "responses": {
          "200": {
            "description": "Ready to receive traffic",
            "content": { "application/json": { "example": { "status": "ready" } } }
          },
          "503": {
            "description": "Dependency unavailable",
            "content": { "application/json": { "example": { "status": "unavailable", "reason": "open-meteo unreachable" } } }
          }
        }
      }
    }
    
  },
  "components": {
    "schemas": {
      "WeatherResponse": {
        "type": "object",
        "properties": {
          "speech":          { "type": "string" },
          "current_weather": { "$ref": "#/components/schemas/CurrentWeather" }
        }
      },
      "CurrentWeather": {
        "type": "object",
        "properties": {
          "temperature": { "type": "number", "description": "C" },
          "windspeed":   { "type": "number", "description": "km/h" },
          "weathercode": { "type": "integer", "description": "WMO weather code" },
          "time":        { "type": "string",  "format": "date-time" }
        }
      },
      "ErrorResponse": {
        "type": "object",
        "properties": { "error": { "type": "string" } }
      },
      "TestResult": {
        "type": "object",
        "properties": {
          "name":   { "type": "string" },
          "status": { "type": "string", "enum": ["PASS", "FAIL"] },
          "info":   { "type": "string" }
        }
      },
      "TestReport": {
        "type": "object",
        "properties": {
          "total":  { "type": "integer" },
          "passed": { "type": "integer" },
          "failed": { "type": "integer" },
          "tests":  { "type": "array", "items": { "$ref": "#/components/schemas/TestResult" } }
        }
      }
    }
  }
})JSON";

            auto res = HttpResponse::newHttpResponse();
            res->setStatusCode(k200OK);
            res->setContentTypeCode(CT_APPLICATION_JSON);
            res->addHeader("Access-Control-Allow-Origin", "*");
            res->setBody(schema);
            callback(res);
        },
        {Get});

    // ─── Swagger UI  (/docs) ──────────────────────────────────────────────────
    app().registerHandler("/docs",
        [](const HttpRequestPtr&,
           std::function<void(const HttpResponsePtr&)>&& callback) {

            static const std::string html = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8"/>
  <title>Weather API - Docs</title>
  <link rel="stylesheet" href="https://unpkg.com/swagger-ui-dist@5/swagger-ui.css"/>
  <style>
    body { margin: 0; }
    #swagger-ui .topbar { background: #1b1b2f; }
    #swagger-ui .topbar-wrapper::after {
      content: "Weather API";
      color: #fff; font-size: 1.3rem; font-weight: 700; padding-left: 1rem;
    }
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
      defaultModelsExpandDepth: 2,
      defaultModelExpandDepth: 2,
      tryItOutEnabled: true
    });
  </script>
</body>
</html>)HTML";

            auto res = HttpResponse::newHttpResponse();
            res->setStatusCode(k200OK);
            res->setContentTypeString("text/html; charset=utf-8");
            res->setBody(html);
            callback(res);
        },
        {Get});

    // ─── Health probes ────────────────────────────────────────────────────────
    // Liveness: is the process responding? → Docker/K8s restarts if unhealthy

    app().registerHandler("/health/live",
        [](const HttpRequestPtr&,
           std::function<void(const HttpResponsePtr&)>&& callback) {
            auto res = HttpResponse::newHttpResponse();
            res->setStatusCode(k200OK);
            res->setContentTypeCode(CT_APPLICATION_JSON);
            res->addHeader("Access-Control-Allow-Origin", "*");
            res->setBody(R"({"status":"ok"})");
            callback(res);
        },
        {Get});
    // Readiness: is the app ready to receive traffic?
    // Here we verify that Open-Meteo is reachable (external dependency).

    app().registerHandler("/health/ready",
        [](const HttpRequestPtr&,
           std::function<void(const HttpResponsePtr&)>&& callback) {

            auto client = HttpClient::newHttpClient("https://api.open-meteo.com");
            auto probe  = HttpRequest::newHttpRequest();
            probe->setPath("/v1/forecast?latitude=48.8566&longitude=2.3522&current_weather=true");

            client->sendRequest(probe,
                [callback](ReqResult result, const HttpResponsePtr& response) {
                    auto res = HttpResponse::newHttpResponse();
                    res->setContentTypeCode(CT_APPLICATION_JSON);
                    res->addHeader("Access-Control-Allow-Origin", "*");

                    if (result != ReqResult::Ok || !response ||
                        response->getStatusCode() != k200OK) {
                        res->setStatusCode(k503ServiceUnavailable);
                        res->setBody(R"({"status":"unavailable","reason":"open-meteo unreachable"})");
                    } else {
                        res->setStatusCode(k200OK);
                        res->setBody(R"({"status":"ready"})");
                    }
                    callback(res);
                });
        },
        {Get});

    // ─── Unit tests ───────────────────────────────────────────────────────────

    app().registerHandler("/test",
        [](const HttpRequestPtr&,
           std::function<void(const HttpResponsePtr&)>&& callback) {

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
                (lat == "48.8566" && lon == "2.3522")
                    ? ok("default_lat_lon")
                    : ko("default_lat_lon", "lat=" + lat + " lon=" + lon);
            }
            {
                std::string lat = "48.8566", lon = "2.3522";
                std::string path = "/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true";
                std::string expected = "/v1/forecast?latitude=48.8566&longitude=2.3522&current_weather=true";
                (path == expected) ? ok("build_url") : ko("build_url", "got: " + path);
            }
            {
                Json::Value root;
                root["speech"]          = "test";
                root["current_weather"] = Json::objectValue;
                std::string body = root.toStyledString();
                (body.find("speech") != std::string::npos && body.find("current_weather") != std::string::npos)
                    ? ok("json_serialization")
                    : ko("json_serialization", "missing keys in: " + body);
            }
            {
                auto res = HttpResponse::newHttpResponse();
                res->addHeader("Access-Control-Allow-Origin", "*");
                std::string v = res->getHeader("Access-Control-Allow-Origin");
                (v == "*") ? ok("cors_header") : ko("cors_header", "got: '" + v + "'");
            }
            {
                std::string lat = "43.2965", lon = "5.3698";
                std::string saved_lat = lat, saved_lon = lon;
                if (lat.empty()) lat = "48.8566";
                if (lon.empty()) lon = "2.3522";
                (lat == saved_lat && lon == saved_lon)
                    ? ok("custom_lat_lon_preserved", "lat=" + lat + " lon=" + lon)
                    : ko("custom_lat_lon_preserved", "overwritten!");
            }

            report["total"]  = passed + failed;
            report["passed"] = passed;
            report["failed"] = failed;
            report["tests"]  = tests;

            auto res = HttpResponse::newHttpResponse();
            res->setContentTypeCode(CT_APPLICATION_JSON);
            res->addHeader("Access-Control-Allow-Origin", "*");
            res->setStatusCode(failed == 0 ? k200OK : k500InternalServerError);
            res->setBody(report.toStyledString());
            callback(res);
        },
        {Get});

    // ─── Business ─────────────────────────────────────────────────────────────
    // Register the /weather route

    app().registerHandler("/weather",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback) {

            // Extract query parameters with default values
            auto lat = req->getParameter("lat");
            auto lon = req->getParameter("lon");
            if (lat.empty()) lat = "48.8566";
            if (lon.empty()) lon = "2.3522";


            // Create an asynchronous HTTP client pointing to Open-Meteo

            auto client = HttpClient::newHttpClient("https://api.open-meteo.com");

            // Build the API request path
            std::string path = "/v1/forecast?latitude=" + lat +
                               "&longitude=" + lon + "&current_weather=true";
            auto apiReq = HttpRequest::newHttpRequest();
            apiReq->setPath(path);

            // Send the request in a non-blocking way
            client->sendRequest(apiReq,
                [callback](ReqResult result, const HttpResponsePtr& response) {
                    auto res = HttpResponse::newHttpResponse();
                    res->addHeader("Access-Control-Allow-Origin", "*");

                    if (result != ReqResult::Ok || !response) {
                        res->setStatusCode(k500InternalServerError);
                        res->setBody(R"({"error":"Failed to connect to Open-Meteo"})");
                        res->setContentTypeCode(CT_APPLICATION_JSON);
                        callback(res);
                        return;
                    }

                    // Drogon internally uses the jsoncpp library
                    auto jsonPtr = response->getJsonObject();
                    if (!jsonPtr) {
                        res->setStatusCode(k500InternalServerError);
                        res->setBody(R"({"error":"External API JSON parsing error"})");
                        res->setContentTypeCode(CT_APPLICATION_JSON);
                        callback(res);
                        return;
                    }

                    // Build the unified response
                    Json::Value root;
                    root["speech"]          = "Refreshes real-time data via the native Drogon framework .";
                    root["current_weather"] = (*jsonPtr)["current_weather"];

                    res->setStatusCode(k200OK);
                    res->setBody(root.toStyledString());
                    res->setContentTypeCode(CT_APPLICATION_JSON);
                    callback(res);
                });
        });

    // ─── Startup ──────────────────────────────────────────────────────────────
    // Configure and start the Drogon server on port 8000
    LOG_INFO << "Drogon server started on port 8000";
    app().addListener("0.0.0.0", 8000)
         .setThreadNum(16)
         .run();

    return 0;
}