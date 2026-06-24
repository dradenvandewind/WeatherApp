BUILD_DIR = build

all: configure build

configure:
	cmake -B $(BUILD_DIR) -S .

build:
	cmake --build $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

run:
	./$(BUILD_DIR)/weather_app

.PHONY: all configure build clean run