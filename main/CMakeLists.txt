idf_component_register(
  SRCS "flight-computer.c" "time.c" "flash-storage.c" "network.c" "models/telemetry.pb-c.c"
  INCLUDE_DIRS "."
  PRIV_REQUIRES bno055 protobuf-c nvs_flash
)
