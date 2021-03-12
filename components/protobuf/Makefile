install: libprotobuf.a

include $(IDF_PATH)/make/project.mk

protobuf:
	mkdir -p ../../main/models/
	protoc --proto_path=../../models --cpp_out=../../main/models/ logs.proto peripherals.proto

libprotobuf.a:
	pushd ./protobuf && \
	./autogen.sh && \
	./configure \
		--prefix=$(PWD) \
		--host=xtensa-esp32-elf \
		--enable-cross-compile \
		--with-protoc=/usr/local/bin/protoc \
		AS=xtensa-esp32-elf-as \
		AR=xtensa-esp32-elf-ar \
		RANLIB=xtensa-esp32-elf-ranlib \
		LD=xtensa-esp32-elf-ld \
		LDXX=xtensa-esp32-elf-g++ \
		CC=xtensa-esp32-elf-gcc \
		CCFLAGS="-pthread -lpthread -lm -std=c++11 -mlongcalls -v" \
		CPP=xtensa-esp32-elf-cpp \
		CXX=xtensa-esp32-elf-g++ \
		CXXFLAGS="-std=c++11 -fexceptions -mlongcalls -Wl,--no-as-needed" && \
		make pthread_cflags='-pthread -lpthread -dgoogle_protobuf_heap_check_draconian' && make install && \
	popd
