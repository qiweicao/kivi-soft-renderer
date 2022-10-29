#ifndef CAMERA_H
#define CAMERA_H

#include "math.hpp"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.05f;
const float SENSITIVITY = 0.1f;

class Camera
{
public:
  Camera(vec3f eye, vec3f target, vec3f up, float aspect);

  vec3f eye;
  vec3f target;
  vec3f up;
  float aspect;

  vec3f x;
  vec3f y;
  vec3f z;

  float yaw;
  float pitch;

  // float moveSpeed;
  // float mouseSensitivity;
};

void processInput(Camera &camera);
mat4f getModelMatrix();
mat4f getViewMatrix(vec3f eye,vec3f target, vec3f up);
mat4f getProjectMatrix(float eye_fov, float aspect_ratio, float zNear, float zFar);

#endif