#ifndef SHADER_H
#define SHADER_H

#include "linear_algebra.h"
#include "camera.h"
#include "model.h"

template<class T>
T interpolate(float alpha, float beta, float gamma, const T &vert1, const T &vert2, const T &vert3, float weight)
{
  return (vert1 * alpha + vert2 * beta + vert3 * gamma) * (1.0f / weight);
}

vec4f toVec4(vec3f &v, float w);

float float_clamp(float f, float min, float max);


typedef struct
{
  mat4f modelMatrix;
  mat4f viewMatrix;
  mat4f projectMatrix;
  mat4f mvMatrix;
  mat4f mvpMatrix;

  Camera *camera;
  Model *model;

  vec3f screenPos[3];

  vec3f outFragPos[3];
  vec3f outNormal[3];
  vec2f outTexCoords[3];

  int isInCone;
} shaderPayload;


class IShader
{
public:
  virtual void vertexShader(int nface)=0;
  virtual Color fragmentShader(float alpha, float beta, float gamma)=0;
  shaderPayload payload;
};

class PhongShader:public IShader
{
public:
  void vertexShader(int nface);
  Color fragmentShader(float alpha, float beta, float gamma);
};

#endif
