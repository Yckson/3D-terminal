.PHONY: dependencies build run convert watch clean help all

# Variables
COMPILER = g++
CFLAGS = -O3
LIBS = -lncurses
TARGET = 3D
SOURCE = main.cpp
OUTPUT_DIR = output
VIDEO_OUTPUT = output.mp4
FRAMES_PATTERN = $(OUTPUT_DIR)/%d.ppm
OBJ_FILE ?= model.obj

# Default target
all: help

help:
	@echo "3D Terminal - Makefile targets:"
	@echo ""
	@echo "  make dependencies  - Install required dependencies"
	@echo "  make build         - Compile the C++ program"
	@echo "  make run           - Run the program (requires OBJ_FILE parameter)"
	@echo "  make convert       - Convert PPM frames to MP4 video"
	@echo "  make watch         - Play the generated video"
	@echo "  make pipeline      - Full pipeline (build + run + convert + watch)"
	@echo "  make clean         - Remove compiled files and video output"
	@echo ""
	@echo "Example usage:"
	@echo "  make dependencies"
	@echo "  make build"
	@echo "  make run OBJ_FILE=path/to/model.obj"
	@echo "  make pipeline OBJ_FILE=path/to/model.obj"

# Install dependencies
dependencies:
	@echo "Installing dependencies..."
	@echo "This script requires sudo for system package installation"
	sudo apt-get update
	sudo apt-get install -y build-essential libncurses-dev ffmpeg mpv
	@echo "Dependencies installed successfully!"

# Build the project
build: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "Compiling $(SOURCE) into $(TARGET)..."
	$(COMPILER) $(CFLAGS) ./$(SOURCE) -o $(TARGET) $(LIBS)
	@echo "Build completed! Executable: $(TARGET)"

# Run the program
run: $(TARGET)
	@if [ -z "$(OBJ_FILE)" ]; then \
		echo "Error: OBJ_FILE parameter is required!"; \
		echo "Usage: make run OBJ_FILE=path/to/model.obj"; \
		exit 1; \
	fi
	@if [ ! -f "$(OBJ_FILE)" ]; then \
		echo "Error: File '$(OBJ_FILE)' not found!"; \
		exit 1; \
	fi
	@echo "Running $(TARGET) with model: $(OBJ_FILE)..."
	./$(TARGET) "$(OBJ_FILE)"

# Convert PPM frames to MP4
convert:
	@if [ ! -d "$(OUTPUT_DIR)" ] || [ -z "$$(ls -A $(OUTPUT_DIR)/*.ppm 2>/dev/null)" ]; then \
		echo "Error: No PPM files found in $(OUTPUT_DIR)/"; \
		echo "Run 'make run' first to generate frames"; \
		exit 1; \
	fi
	@echo "Converting PPM frames to video..."
	cd $(OUTPUT_DIR) && ffmpeg -y -i %d.ppm -r 60 ../$(VIDEO_OUTPUT)
	@echo "Video created: $(VIDEO_OUTPUT)"

# Watch/play the video
watch:
	@if [ ! -f "$(VIDEO_OUTPUT)" ]; then \
		echo "Error: $(VIDEO_OUTPUT) not found!"; \
		echo "Run 'make convert' first"; \
		exit 1; \
	fi
	@echo "Playing $(VIDEO_OUTPUT)..."
	mpv ./$(VIDEO_OUTPUT)

# Full pipeline: build -> run -> convert -> watch
pipeline: build
	@if [ -z "$(OBJ_FILE)" ]; then \
		echo "Error: OBJ_FILE parameter is required!"; \
		echo "Usage: make pipeline OBJ_FILE=path/to/model.obj"; \
		exit 1; \
	fi
	@echo "Starting full pipeline..."
	$(MAKE) run OBJ_FILE=$(OBJ_FILE)
	$(MAKE) convert
	$(MAKE) watch

# Clean build artifacts and output
clean:
	@echo "Cleaning up..."
	rm -f $(TARGET)
	rm -f $(VIDEO_OUTPUT)
	@echo "Cleanup completed!"

# Clean everything including frames
clean-all: clean
	@echo "Removing all generated PPM frames..."
	rm -f $(OUTPUT_DIR)/*.ppm
	@echo "All cleaned!"
