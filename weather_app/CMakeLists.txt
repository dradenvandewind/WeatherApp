cmake_minimum_required(VERSION 3.14)
project(WeatherAppDrogon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Recherche de Drogon et de ses dépendances (JsonCpp, etc.)
find_package(Drogon REQUIRED)

add_executable(drogon_weather src/main.cpp)
target_link_libraries(drogon_weather PRIVATE Drogon::Drogon)