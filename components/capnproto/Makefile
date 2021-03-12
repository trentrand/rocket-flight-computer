install: libcapnproto.a

include $(IDF_PATH)/make/project.mk

libcapnproto.a:
	pushd ./capnproto/c++/ && \
	autoreconf -i && \
	./configure \
		--prefix=$(PWD) \
		--host=xtensa-esp32-elf \
		AS=xtensa-esp32-elf-as \
		AR=xtensa-esp32-elf-ar \
		RANLIB=xtensa-esp32-elf-ranlib \
		LD=xtensa-esp32-elf-ld \
		LDXX=xtensa-esp32-elf-g++ \
		CC=xtensa-esp32-elf-gcc \
		CCFLAGS="-std=c++14" \
		CPP=xtensa-esp32-elf-cpp \
		CXX=xtensa-esp32-elf-g++ \
		CXXFLAGS="-DCAPNP_LITE=1 -DIOV_MAX=16 -std=c++14" && \
	make && make install && \
	popd
