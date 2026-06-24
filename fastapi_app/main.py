from fastapi import FastAPI, HTTPException
import asyncio
import json
import os

app = FastAPI(title="Weather API Gateway")

# Path to the C++ binary shared via a volume or copied
BINARY_PATH = "/app_bin/weather_app"

@app.get("/weather")
async def get_weather(lat: float = 48.11, lon: float = -1.67):
    if not os.path.exists(BINARY_PATH):
        raise HTTPException(status_code=500, detail="C++ binary not found")

    # Asynchronously run the C++ binary with GPS coordinates
    proc = await asyncio.create_subprocess_exec(
        BINARY_PATH, str(lat), str(lon),
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE
    )

    stdout, stderr = await proc.communicate()

    if proc.returncode != 0:
        raise HTTPException(status_code=500, detail=f"C++ application error: {stderr.decode()}")

    try:
        # Load the JSON returned by the C++ binary to return it cleanly
        weather_data = json.loads(stdout.decode().strip())
        return {
            "status": "success",
            "location": {"latitude": lat, "longitude": lon},
            "current_weather": weather_data
        }
    except json.JSONDecodeError:
        raise HTTPException(status_code=500, detail="Error parsing C++ output data")