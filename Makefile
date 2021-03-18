PROJECT_NAME := rocket-flight-computer

include $(IDF_PATH)/make/project.mk

compile-protos:
	capnp compile -o"./components/c-capnproto/c-capnproto/capnpc-c" \
		./main/models/telemetry.capnp
