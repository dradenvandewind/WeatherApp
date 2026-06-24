from fastapi import FastAPI, HTTPException, Query
import asyncio
import json
import os

app = FastAPI(title="Weather API Gateway Behind Nginx")

BINARY_PATH = "/app_bin/weather_app"

@app.get("/weather")
async def get_weather(
    lat: float = Query(48.8566),
    lon: float = Query(2.3522)
):
    if not os.path.exists(BINARY_PATH):
        raise HTTPException(status_code=500, detail="Moteur C++ indisponible.")

    process = await asyncio.create_subprocess_exec(
        BINARY_PATH, str(lat), str(lon),
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE
    )
    stdout, stderr = await process.communicate()

    if process.returncode != 0:
        raise HTTPException(status_code=500, detail="Échec moteur C++")

    try:
        weather_json = json.loads(stdout.decode().strip())
        return {
            "speech": "Données actualisées depuis le moteur C++ natif.",
            "current_weather": weather_json
        }
    except json.JSONDecodeError:
        raise HTTPException(status_code=500, detail="Erreur parsing JSON")