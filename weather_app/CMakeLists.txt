cmake_minimum_required(VERSION 3.14)
project(WeatherApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(weather_app src/main.cpp)

target_link_libraries(weather_app PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)