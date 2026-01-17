.PHONY: build run 
MAKEFLAGS += --no-print-directory
build:
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build build

run:
	./build/sim
