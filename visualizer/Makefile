CFLAGS=`pkg-config --libs --cflags raylib 'libprotobuf-c >= 1.0.0' libserialport`

test: build run

install: protobufc_install libserialport_install

run: build
	./build/RunVisualizer ${ESPPORT}

build: clean
	mkdir build
	cc $(CFLAGS) ./src/main.c ./lib/models/telemetry.pb-c.c -g -o ./build/RunVisualizer

clean:
	rm -rf ./build

compile-protos:
	protoc --proto_path=../models --c_out=./lib/models/ telemetry.proto

protobufc_install:
	cd lib/protobuf-c/ && \
	./autogen.sh && ./configure --prefix=`pwd`/build && \
	make && make install

libserialport_install:
	cd lib/libserialport/ && \
	./autogen.sh && ./configure && \
	make && make install
