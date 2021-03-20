PROJECT_NAME := rocket-flight-computer

include $(IDF_PATH)/make/project.mk

compile-protos:
	echo "Compiled protocol buffers for flight computer" && \
	protoc --c_out=./main/ \
		./models/telemetry.proto && \
	echo "Compiled protocol buffers for visualizer" && \
	protoc --c_out=./visualizer/src/ \
		./models/telemetry.proto
