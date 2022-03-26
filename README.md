# rocket-flight-computer
🧑🏻‍🚀  Flight computer for model rockets

## Flight computer

The firmware is written with [ESP-IDF](https://github.com/espressif/esp-idf) development framework and ran on an [ESP-32](https://en.wikipedia.org/wiki/ESP32) microcontroller.

External sensors are connected to collect telemetry data:
- BNO055 Absolute Orientation Sensor (9-dof)
- BMP388 Precision Barometric Pressure and Altimeter

Telemetry data is serialized into a binary wire format using [protocol buffers](https://en.wikipedia.org/wiki/Protocol_Buffers).

Ground control software can log telemetry data broadcasted wirelessly by the flight controller.

On-board software can use the telemetry data stream as process variables in a PID control system.

## Visualizer

A companion visualizer application allows real-time visualization of the model rocket body.

Telemetry data is streamed over USB and processed as updates to a 3D graphical scene.

The visualizer uses [raylib](https://www.raylib.com/) for rendering and [protobuf-c](https://github.com/protobuf-c/protobuf-c) for packet deserialization.
