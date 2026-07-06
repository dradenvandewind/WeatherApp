#include "WeatherApp.hpp"
#include "OpenMeteoProvider.hpp"
#include <memory>

int main() {
    auto provider = std::make_unique<OpenMeteoProvider>();
    WeatherApp app(std::move(provider));
    app.initAndRun("0.0.0.0", 8000, 16);
    return 0;
}