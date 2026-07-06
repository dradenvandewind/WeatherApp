cmake_minimum_required(VERSION 3.14)
project(WeatherAppDrogon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)

set(SOURCES
    src/main.cpp
    src/WeatherApp.cpp
)

add_executable(drogon_weather ${SOURCES})

target_include_directories(drogon_weather PRIVATE src) 

target_link_libraries(drogon_weather PRIVATE Drogon::Drogon)