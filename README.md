#Weather App

Development of a minimal C++ weather application that retrieves data from a public API (such as Open-Meteo, which does not require an API key). 
The application is containerized using Docker and orchestrated with Docker Compose.

This project will use the following libraries:

libcurl: for performing HTTP requests to the API.
nlohmann/json: a modern C++ library for parsing and handling JSON responses.


"""
Client (HTTP)
    │
    ▼
FastAPI Gateway
(API REST / validation / orchestration)
    │
    ▼ (subprocess / RPC / exec optimisé)
C++ Weather Service
(libcurl + nlohmann/json)
    │
    ▼
Open-Meteo API (HTTP externe)
    │
    ▼
JSON brut
    │
    ▲
C++ service (validation + structuration)
    │
    ▲
FastAPI (normalisation + formatting)
    │
    ▼
Client response (JSON propre)


"""

Rôle de chaque composant
1. FastAPI – API Gateway / Speech Layer

FastAPI

Responsabilités :
Responsabilités :

Exposition de l’API HTTP publique
Validation des coordonnées GPS
Orchestration du backend C++
Normalisation des réponses
Gestion des erreurs et timeouts
Interface unique côté client


2. Service C++ – Weather Compute Microservice

Responsabilités :

Exécution métier “bas niveau”
Appel HTTP vers Open-Meteo via libcurl
Parsing et validation JSON via nlohmann/json
Transformation des données météo
Retour structuré vers FastAPI

3. API externe – Open-Meteo

Open-Meteo

Responsabilités :

Fourniture des données météo brutes
Source externe sans clé API


Architecture logique (microservices)


"""
                ┌──────────────────────┐
                │      Client          │
                └─────────┬────────────┘
                          │ HTTP
                          ▼
        ┌──────────────────────────────────┐
        │        FastAPI Gateway           │
        │  - Auth / validation             │
        │  - Orchestration                 │
        │  - Response formatting           │
        └─────────────┬────────────────────┘
                      │ exec / IPC
                      ▼
        ┌──────────────────────────────────┐
        │   C++ Weather Microservice       │
        │  - libcurl HTTP client           │
        │  - JSON parsing (nlohmann)       │
        │  - business transformation       │
        └─────────────┬────────────────────┘
                      │ HTTP
                      ▼
        ┌──────────────────────────────────┐
        │       Open-Meteo API             │
        └──────────────────────────────────┘
"""


https://api.open-meteo.com/v1/forecast?latitude=48.11&longitude=-1.67&current_weather=true

'''
weather-app/
├── docker-compose.yml
├── cpp-app/
│   ├── Dockerfile
│   ├── CMakeLists.txt
│   ├── Makefile
│   └── src/
│       └── main.cpp
└── fastapi-app/
    ├── Dockerfile
    ├── main.py
    └── requirements.txt
'''

# Build and Run
docker compose up --build




# Api Documentation
http://127.0.0.1:8000/docs


#Test

curl "http://localhost:8000/weather?lat=48.39&lon=-4.48"


