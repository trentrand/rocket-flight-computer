CFLAGS=`pkg-config --libs --cflags raylib`

run: build
	./build/RunVisualizer

build: clean
	mkdir build
	cc $(CFLAGS) ./src/main.c -g -o ./build/RunVisualizer

clean:
	rm -rf ./build