#include <drogon/drogon.h>
#include <iostream>

using namespace drogon;

int main() {
    // Register the /weather route
    app().registerHandler("/weather", [](const HttpRequestPtr& req, 
                                         std::function<void (const HttpResponsePtr &)> &&callback) {
        
        // Extract query parameters with default values
        auto lat = req->getParameter("lat");
        auto lon = req->getParameter("lon");
        if (lat.empty()) lat = "48.8566";
        if (lon.empty()) lon = "2.3522";

        // Create an asynchronous HTTP client pointing to Open-Meteo
        auto client = HttpClient::newHttpClient("https://api.open-meteo.com");
        
        // Build the API request path
        std::string path = "/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true";
        auto apiReq = HttpRequest::newHttpRequest();
        apiReq->setPath(path);

        // Send the request in a non-blocking way
        client->sendRequest(apiReq, [callback](ReqResult result, const HttpResponsePtr &response) {
            auto res = HttpResponse::newHttpResponse();
            res->addHeader("Access-Control-Allow-Origin", "*"); // CORS security

            if (result != ReqResult::Ok || !response) {
                res->setStatusCode(k500InternalServerError);
                res->setBody("{\"error\": \"Failed to connect to Open-Meteo\"}");
                res->setContentTypeCode(CT_APPLICATION_JSON);
                callback(res);
                return;
            }

            // Drogon internally uses the jsoncpp library
            auto jsonPtr = response->getJsonObject();
            if (!jsonPtr) {
                res->setStatusCode(k500InternalServerError);
                res->setBody("{\"error\": \"External API JSON parsing error\"}");
                callback(res);
                return;
            }

            // Build the unified response
            Json::Value root;
            root["speech"] = "Refreshes real-time data via the native Drogon framework (C++).";
            root["current_weather"] = (*jsonPtr)["current_weather"];

            res->setStatusCode(k200OK);
            res->setBody(root.toStyledString());
            res->setContentTypeCode(CT_APPLICATION_JSON);
            callback(res);
        });
    });

    // Configure and start the Drogon server on port 8000
    LOG_INFO << "Drogon server started on port 8000";
    app().addListener("0.0.0.0", 8000)
         .setThreadNum(16) // Number of threads to handle requests in parallel
         .run();

    return 0;
}