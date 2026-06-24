FROM python:3.11-slim

# FastAPI needs libcurl4 if the C++ binary runs in its environment
RUN apt-get update && apt-get install -y libcurl4 && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY main.py .

CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]