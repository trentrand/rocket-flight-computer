CFLAGS=`pkg-config --libs --cflags 'raylib' 'libprotobuf-c >= 1.0.0'`
LDFLAGS=`pkg-config --libs 'libprotobuf-c >= 1.0.0'`

test: build run

install: protobufc_install

run: build
	./build/RunVisualizer

build: clean
	mkdir build
	cc $(CFLAGS) ./src/main.c -g -o ./build/RunVisualizer

clean:
	rm -rf ./build

protobuf:
	protoc --proto_path=../models --c_out=src/models logs.proto peripherals.proto

protobufc_install:
	pushd lib/protobuf-c/ \
	./autogen.sh && ./configure && \
	make && make install
	popd
