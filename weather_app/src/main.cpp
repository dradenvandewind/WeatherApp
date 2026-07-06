#include "WeatherApp.hpp"

int main() {
    WeatherApp app;
    app.initAndRun("0.0.0.0", 8000, 16);
    return 0;
}