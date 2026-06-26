BUILD_DIR = build

all: configure build

configure:
	cmake -B $(BUILD_DIR) -S .

build:
	cmake --build $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	
# Lance les tests via Docker Compose
test:
	docker compose up --build integration-tests --exit-code-from integration-tests

.PHONY: all configure build clean test