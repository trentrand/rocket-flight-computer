PROJECT_NAME := rocket-flight-computer

include $(IDF_PATH)/make/project.mk

compile-protos:
	protoc --c_out=./main/ \
		./models/telemetry.proto
