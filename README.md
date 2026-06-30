#Weather App

```text
Development of a minimal C++ weather application that retrieves data from a public API (such as Open-Meteo, which does not require an API key). 
https://api.open-meteo.com/v1/forecast?latitude=48.11&longitude=-1.67&current_weather=true

The application is containerized using Docker and orchestrated with Docker Compose.

```


## Role of Each Component

```text
┌─────────────────────────────────────────────────────────────────┐
│ Router                                                          │
│  · registers handlers via registerHandler()                     │
│  · dispatches each HTTP request to the correct handler          │
│  · manages threading (16 non-blocking I/O threads)              │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ /weather                                                        │
│  · reads lat / lon query params                                 │
│  · applies default values (Paris 48.8566 / 2.3522)              │
│  · builds the Open-Meteo request URL                            │
│  · triggers async HTTP call via HttpClient                      │
│  · extracts current_weather from the returned JSON              │
│  · formats and returns the business response                    │
│  · sets CORS header Access-Control-Allow-Origin: *              │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ /health/live                                                    │
│  · checks only that the Drogon process is responding            │
│  · no I/O, no external dependency                               │
│  · always returns 200 {"status":"ok"} if the thread is running  │
│  · used by Docker HEALTHCHECK and K8s livenessProbe             │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ /health/ready                                                   │
│  · sends a real HTTP request to Open-Meteo                      │
│  · returns 200 {"status":"ready"} if the dependency responds    │
│  · returns 503 {"status":"unavailable"} otherwise               │
│  · used by K8s readinessProbe to remove pod from LB if down     │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ /docs                                                           │
│  · serves a static HTML page embedding Swagger UI (unpkg CDN)   │
│  · points to /openapi.json to load the schema                   │
│  · allows interactive API testing from a browser                │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ /openapi.json                                                   │
│  · exposes the full OpenAPI 3.0 schema of the API               │
│  · describes all routes, parameters, responses and schemas      │
│  · consumed by Swagger UI and any OpenAPI-compatible client     │
│  · sets CORS header to be readable cross-origin                 │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ /test                                                           │
│  · runs 5 internal unit tests with no network calls             │
│  · covers: lat/lon defaults, URL building, JSON serialization,  │
│    CORS header, custom param preservation                       │
│  · returns 200 if all pass, 500 if at least one fails           │
│  · CI-friendly: usable with curl -f in a pipeline               │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ HttpClient                                                      │
│  · creates an async HTTP client to api.open-meteo.com           │
│  · sends requests in a non-blocking manner                      │
│  · shared by /weather AND /health/ready                         │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ jsoncpp  (Json::Value)                                          │
│  · parses the JSON body returned by Open-Meteo                  │
│  · builds the business response object (speech + weather)       │
│  · serializes the final response with toStyledString()          │
└─────────────────────────────────────────────────────────────────┘

```


## Architecture logique (microservices)


```text
				Client / Browser
    		    │
        		│ HTTP :8080
        		▼
┌─────────────────────────────────────────────────────┐
│              Drogon C++  ·  0.0.0.0:8080            │
│                                                     │
│                                                     │
│                      Router                         │
│               (registerHandler · async)             │
│                                                     │
│   ┌──────────┬──────────────┬──────────┬            │
│   │          │              │          │            │
│   ▼          ▼              ▼          ▼            │
│ /weather  /health        /health     /docs          │
│           /live          /ready      /openapi       │
│             │              │        .json           │
│  lat·lon    │ {"status"    │                        │
│  → Weather  │  :"ok"}      │ ping                   │
│             │              │ Open-Meteo             │
│             │         ─────┘                        │
│             │        │                              │
│         ┌───┴────┐  ┌┴─────────┐  ┌──────────────┐  │
│         │jsoncpp │  │HttpClient│  │  CORS header │  │
│         │Json::  │  │async ·   │  │  Allow-Origin│  │
│         │Value   │  │non-block │  │      *       │  │
│         └────────┘  └────┬─────┘  └──────────────┘  │
└─────────────────────────┼───────────────────────────┘
                          │ HTTP
                          ▼
              ┌─────────────────────────┐
              │     Open-Meteo API      │
              │  api.open-meteo.com     │
              │  /v1/forecast           │
              └─────────────────────────┘

return code
  200  → Business success / Test OK
  503  → Open-Meteo unavailable (/health/ready)
  500  → Internal error / JSON parsing failed
└─────────────────────────────────────────────────────────────────┘
```


https://api.open-meteo.com/v1/forecast?latitude=48.11&longitude=-1.67&current_weather=true


## Project structure

```text
weather-app/
├── Readme.md
├── docker-compose.yml
├── Dockerfile.tests
├── weather_app/
│   ├── Dockerfile
│   ├── CMakeLists.txt
│   ├── Makefile
│   └── src/
│       └── main.cpp
└── nginx/
│    ├── default.conf
│    ├── html/
│        └── index.html
└── grafana/   
│      ├── grafana/provisioning/datasources
│	   │								├── dashboards.yml
│	   │								├── node-exporter-full.json
│	   │								└── dashboard-container.json
│      │
│      └── grafana/provisioning/datasources/
│									└── datasource.yml
└── monitoring/   
│      └── prometheus.yml
│      
└── tests/   
│      └── requirements.txt
│      └── test_weather.py
└── Iac/
    └── terraform/
	│     └──  main.tf
	│	  └── providers.tf
	│	  └── variables.tf
	│	  └── outputs.tf
    │
    └── ansible
	│      └── ansible.cfg
	│	   └── playbook.yml
	│	   └── templates
    └── Readme.md  
      
/
```

# Build and Run
```bash
docker compose up --build
```

# Open browser  and configure it with gps coordinate
http://127.0.0.1:8080/

# in using  command line

# Test Live
```bash
docker exec drogon_weather_backend curl -s http://localhost:8000/health/live
```
# → {"status":"ok"}


# Test Ready
```bash
curl -X 'GET' 'http://localhost:8080/health/ready' -H 'accept: application/json'
```
```text
{"status":"ready"}
```
# Test Api

```bash
curl http://localhost:8080/api/weather
```
```json
{
	"current_weather" : 
	{
		"interval" : 900,
		"is_day" : 1,
		"temperature" : 34.0,
		"time" : "2026-06-25T09:15",
		"weathercode" : 2,
		"winddirection" : 73,
		"windspeed" : 9.8000000000000007
	},
	"speech" : "Refreshes real-time data via the native Drogon framework (C++)."
}
```
```bash
curl "http://localhost:8080/api/weather?lat=48.39&lon=-4.48"
```
```json
{
	"current_weather" : 
	{
		"interval" : 900,
		"is_day" : 1,
		"temperature" : 34.799999999999997,
		"time" : "2026-06-25T11:45",
		"weathercode" : 0,
		"winddirection" : 180,
		"windspeed" : 10.1
	},
	"speech" : "Refreshes real-time data via the native Drogon framework (C++)."
}
```






# Api Documentation
http://127.0.0.1:8080/docs

# Monitoring Node App
Open your browser,http://localhost:3000
 log in using the credentials “admin:admin,” and skip the step to change the password to a new one

 Select dashboard  http://localhost:3000/dashboards 


#Test

curl "http://localhost:8000/weather?lat=48.39&lon=-4.48"


# Test auto
```bash
$ cd weather_app 
$ make test
```


#Deploy weather app on aws 

You must have an account to retrieve your login credentials and configure the access policy in order to use the platform.

cd Iac
Open Readme.md and follow some instructions



