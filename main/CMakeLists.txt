idf_component_register(
  SRCS "flight-computer.cpp" "models/telemetry.pb-c.c"
  INCLUDE_DIRS "."
  PRIV_REQUIRES BNO055 protobuf-c
)
