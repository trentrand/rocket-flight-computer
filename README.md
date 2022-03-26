# rocket-flight-computer
🧑🏻‍🚀  Flight computer for model rockets

External sensors collect telemetry data:
- BNO055 Absolute Orientation Sensor (9-dof)
- BMP388 Precision Barometric Pressure and Altimeter

Telemetry data is serialized into a binary wire format using [protocol buffers](https://en.wikipedia.org/wiki/Protocol_Buffers).

Ground control can monitor in-flight telemetry data and logs that is broadcasted by the flight controller.

Control systems can use the telemetry datastream as process variables in a PID system.

Firmware is written with ESP-IDF and ran on an ESP-32 microcontroller.
