install: libcapnp_c.a

include $(IDF_PATH)/make/project.mk

libcapnp_c.a:
	pushd ./c-capnproto/ && \
	autoreconf -f -i -s && \
	./configure \
		--prefix=$(PWD) \
		--host=xtensa-esp32-elf \
		--without-gtest \
		AS=xtensa-esp32-elf-as \
		AR=xtensa-esp32-elf-ar \
		RANLIB=xtensa-esp32-elf-ranlib \
		LD=xtensa-esp32-elf-ld \
		LDXX=xtensa-esp32-elf-g++ \
		CC=xtensa-esp32-elf-gcc \
		CFLAGS="-lgcc -lc" \
		CCFLAGS="-std=c++14 -lgcc -c" \
		CPP=xtensa-esp32-elf-cpp \
		CXX=xtensa-esp32-elf-g++ \
		CXXFLAGS="-DIOV_MAX=16 -std=c++14 -c" && \
	make -w && \
	popd

clean:
	rm -rf ./build
