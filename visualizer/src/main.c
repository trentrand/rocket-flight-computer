#include <stdio.h>
#include <stdlib.h>
#include "../lib/raylib.h"
#include "../lib/rlgl.h"
#include "../lib/raymath.h"
#include "../lib/models/telemetry.pb-c.h"
#include "./drivers/serial-interface.c"

int main(int argc, char *argv[argc+1]) {
  setup_serial_stream("/dev/cu.usbserial-0001");
  uint8_t readBuffer[128];
  int readCount = 0;
  while(readCount <= 250) {
    int numberOfBytesRead = read_serial_stream(readBuffer, sizeof(readBuffer));
    printf("Read %i bytes. Received message: %s\n\n", numberOfBytesRead, readBuffer);

    Telemetry *msg = telemetry__unpack(NULL, numberOfBytesRead, readBuffer);
    if (msg == NULL) {
      fprintf(stderr, "error unpacking incoming message\n");
      exit(1);
    }

    printf("Received timestamp: %llu\n", msg->timestampstart);
    printf("\n");

    telemetry__free_unpacked(msg, NULL);

    readCount += 1;
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

  close_serial_stream();
  CloseWindow();

  return EXIT_SUCCESS;
}
