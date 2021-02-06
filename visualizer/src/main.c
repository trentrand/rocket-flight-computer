#include "stdlib.h"
#include "raylib.h"
#include "rlgl.h"

int main(int argc, char *argv[argc+1]) {
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "Automated Rocket Control Visualizer");

  Camera camera = { 0 };
  camera.position = (Vector3){ 250.0f, 200.0f, 250.0f };
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.type = CAMERA_PERSPECTIVE;

  float pitch = 0.0f;
  float roll = 0.0f;
  float yaw = 0.0f;

  SetCameraMode(camera, CAMERA_FREE);

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    UpdateCamera(&camera);

    if (IsKeyDown('Z')) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };

    if (IsKeyDown('1')) camera.position = (Vector3){ 250.0f, 250.0f, 250.0f };
    if (IsKeyDown('2')) camera.position = (Vector3){ 250.0f, 350.0f, 250.0f };
    if (IsKeyDown('3')) camera.position = (Vector3){ 250.0f, 450.0f, 250.0f };

    BeginDrawing();
    {
      ClearBackground((Color){ 242, 242, 242, 0 });

      BeginMode3D(camera);
      {
        DrawCylinder((Vector3){ 0.0f, 50.0f, 0.0f }, 5.0f, 5.0f, 50.f, 100, BLACK);

        DrawGrid(20, 10.0f);
        rlPushMatrix();
          rlRotatef(90.0f, 0.0f, 0.0f, 1.0f);
          rlTranslatef(100.0f, 100.0f, 0.0f);
          DrawGrid(20, 10.0f);
          rlRotatef(-90.0f, 0.0f, 0.0f, 1.0f);

          rlRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
          rlTranslatef(100.0f, 100.0f, 0.0f);
          DrawGrid(20, 10.0f);
          rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        rlPopMatrix();
      }
      EndMode3D();

      DrawFPS(10, 10);
    }
    EndDrawing();
  }

  CloseWindow();

  return EXIT_SUCCESS;
}
