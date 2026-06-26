import os
import pytest
import requests

# Use the environment variable if it exists (Docker); otherwise, use localhost (host)
BASE_URL = os.getenv("BASE_URL", "http://localhost:8080/api")


def test_weather_default_coordinates():
    """Verify that the route works without parameters (default Paris coordinates)."""
    response = requests.get(f"{BASE_URL}/weather")
    
    assert response.status_code == 200
    data = response.json()
    
    assert "speech" in data
    assert "current_weather" in data
    assert "temperature" in data["current_weather"]
    assert "windspeed" in data["current_weather"]

def test_weather_custom_coordinates():
    """Verify proper behavior with specific coordinates (for example Rennes)."""
    params = {"lat": 48.11, "lon": -1.67}
    response = requests.get(f"{BASE_URL}/weather", params=params)
    
    assert response.status_code == 200
    data = response.json()
    
    assert "Drogon" in data["speech"]
    assert isinstance(data["current_weather"]["temperature"], (int, float))

def test_invalid_params_fallback():
    """Verify that the external API responds or that Drogon handles unusual inputs."""
    params = {"lat": "invalid_string", "lon": "invalid_string"}
    response = requests.get(f"{BASE_URL}/weather", params=params)
    
    # Open-Meteo or Drogon generally apply a fallback or return a 400/500 depending on validation.
    # we verify that we receive a structured JSON response (success or handled error).
    assert "application/json" in response.headers["Content-Type"]
