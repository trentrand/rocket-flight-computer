#include <stdio.h>
#include <stdlib.h>
#include "../lib/raylib.h"
#include "../lib/rlgl.h"
#include "../lib/raymath.h"
#include "../lib/models/telemetry.pb-c.h"
#include "./circular-buffer.c"
#include "./serial-interface.c"
#include "./packet.c"

int main(int argc, char *argv[argc+1]) {
  if (argc < 2 || argc > 3) {
    printf("Usage: %s <serial port>\n", argv[0]);
    return -1;
  }
  char *port_name = argv[1];
  serial_initialize(port_name);

  // TODO remove this buffer, write directly to circular buffer
  size_t temporaryInputBufferLength = 8;
  char* temporaryInputBuffer = malloc(temporaryInputBufferLength * sizeof(char));

  size_t serialBufferLength = 128;
  circular_buffer_t* serialInputBuffer = circular_buffer_initialize(serialBufferLength);

  size_t packetDataLength = 128;
  char* packetData = malloc(packetDataLength * sizeof(char));
  packet_t* packet = packet_initialize(packetData, packetDataLength);

  // TODO don't block visualizer render
  while(true) {
    int numberOfBytesReadBeforeTimeout = serial_read(temporaryInputBuffer, temporaryInputBufferLength);
    int numberOfBytesWritten = circular_buffer_write(serialInputBuffer, temporaryInputBuffer, numberOfBytesReadBeforeTimeout);
    if (numberOfBytesWritten == 0 && numberOfBytesReadBeforeTimeout != 0) {
      printf("Buffer full\n");
    }
    read_packet_payload_from_buffer(serialInputBuffer, packet);
    if (packet->state == COMPLETE) {
      print_packet_payload(packet);
      packet->state = PARTIAL;
      packet->payloadLength = 0;
    }
  }

  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "Automated Rocket Control Visualizer");

  Camera camera = { 0 };
  camera.position = (Vector3){ 250.0f, 200.0f, 250.0f };
  camera.target = (Vector3){ 0.0f, 75.0f, 0.0f };
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.type = CAMERA_PERSPECTIVE;

  Mesh rocketBodyMesh = GenMeshCylinder(3.0f, 30.f, 10);
  Model rocketBody = LoadModelFromMesh(rocketBodyMesh);

  float pitch = 0.0f;
  float roll = 0.0f;
  float yaw = 0.0f;

  SetCameraMode(camera, CAMERA_FREE);

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    UpdateCamera(&camera);

    BeginDrawing();
    {
      ClearBackground((Color){ 242, 242, 242, 0 });

      BeginMode3D(camera);
      {
        DrawCubeV((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 200.0f, -0.1f, 200.0f}, (Color){ 255, 255, 255, 255});
        DrawGrid(20, 10.0f);

        rlPushMatrix();
          rlRotatef(90.0f, 0.0f, 0.0f, 1.0f);
          rlTranslatef(100.0f, 100.0f, 0.0f);

          DrawCubeV((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 200.0f, -0.1f, 200.0f}, (Color){ 255, 255, 255, 255});
          DrawGrid(20, 10.0f);
        rlPopMatrix();

        rlPushMatrix();
          rlRotatef(90.0f, -1.0f, 0.0f, 0.0f);
          rlTranslatef(0.0f, 100.0f, 100.0f);

          DrawCubeV((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 200.0f, -0.1f, 200.0f}, (Color){ 255, 255, 255, 255});
          DrawGrid(20, 10.0f);

          rlRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        rlPopMatrix();

        rocketBody.transform = MatrixRotateXYZ((Vector3){ DEG2RAD * pitch, DEG2RAD * yaw, DEG2RAD * roll });
        DrawModel(rocketBody, (Vector3){ 0.0f, 50.0f, 0.0f }, 1.0f, BLACK);
      }
      EndMode3D();

      DrawText("Rocket Visualizer", (screenWidth / 2) - 100, 10, 20, BLACK);
      DrawFPS(10, 10);
    }
    EndDrawing();
  }

  free(temporaryInputBuffer);
  circular_buffer_free(serialInputBuffer);
  serial_close();

  CloseWindow();

  return EXIT_SUCCESS;
}
