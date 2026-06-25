BUILD_DIR = build

all: configure build

configure:
	cmake -B $(BUILD_DIR) -S .

build:
	cmake --build $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

# Nouvelle commande pour exécuter les tests Pytest
test:
	@echo "=== Lancement des tests d'intégration Pytest ==="
	python3 -m venv tests/.venv
	./tests/.venv/bin/pip install -r tests/requirements.txt
	./tests/.venv/bin/pytest tests/ -v

.PHONY: all configure build clean test