CFLAGS=`pkg-config --libs --cflags raylib`

test: build run

run: build
	./build/RunVisualizer

build: clean
	mkdir build
	cc $(CFLAGS) ./src/main.c -g -o ./build/RunVisualizer

clean:
	rm -rf ./build
