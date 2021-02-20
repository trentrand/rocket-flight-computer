CFLAGS=`pkg-config --libs --cflags raylib`

test: build run

postinstall: protobufc_install

run: build
	./build/RunVisualizer

build: clean
	mkdir build
	cc $(CFLAGS) ./src/main.c -g -o ./build/RunVisualizer

clean:
	rm -rf ./build

protobuf:
	protoc --proto_path=../models --c_out=lib peripherals.proto

protobufc_install:
	cd lib/protobuf-c/ \
	./autogen.sh && ./configure && \
	make && make install
