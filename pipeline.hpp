#ifndef PIPELINE_H
#define PIPELINE_H

#include <vector>
#include <cassert>
#include <algorithm>

#include "win32.h"
#include "math.hpp"
#include "shader.hpp"


void setPixel(int x, int y, const Color color, unsigned char *frameBuffer)
{
  if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
  {
    y = WINDOW_HEIGHT - 1 - y;
    unsigned char *target = &(frameBuffer[y * WINDOW_WIDTH * 4 + x * 4]);
    target[0] = (unsigned char)color.r;
    target[1] = (unsigned char)color.g;
    target[2] = (unsigned char)color.b;
    target = nullptr;
  }
}

void setLine(int x0, int y0, int x1, int y1, const Color color, unsigned char *frameBuffer)
{
  bool steep = false;
  if (std::abs(x0 - x1) < std::abs(y0 - y1))
  {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }
  if (x0 > x1)
  {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  int dx = x1 - x0;
  int dy = y1 - y0;
  float derror = std::abs(dy / float(dx)); //after swap slope<=1
  float error = 0;
  int y = y0;
  for (int x = x0; x <= x1; x++)
  {
    if (steep)
    {
      setPixel(y, x, color, frameBuffer);
    }
    else
    {
      setPixel(x, y, color, frameBuffer);
    }
    error += derror;
    if (error > .5)
    {                          //*2dx float->int
      y += (y1 > y0 ? 1 : -1); //next y judged by error
      error -= 1.;
    }
  }
}

void clearFrameBuffer(int width, int height, unsigned char *framebuffer)
{
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      int index = (i * width + j) * 4;

      framebuffer[index + 2] = 80;
      framebuffer[index + 1] = 56;
      framebuffer[index] = 56;
    }
  }
}

void clearZBuffer(int width, int height, float *zbuffer)
{
  for (int i = 0; i<width*height;i++ )
    zbuffer[i] = std::numeric_limits<float>::max();
}

// vec3f barycentric(std::vector<vec2i> &vertexs, vec2i P) {
//   assert(vertexs.size()==3);
//   vec3f u = vec3f(vertexs[2][0]-vertexs[0][0], vertexs[1][0]-vertexs[0][0], vertexs[0][0]-P[0])
//             ^vec3f(vertexs[2][1]-vertexs[0][1], vertexs[1][1]-vertexs[0][1], vertexs[0][1]-P[1]);
//   if (std::abs(u[2])<1) return vec3f(-1,1,1);
//   return vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
// }

// void setTriangle(std::vector<vec2i> &vertexs,  const Color color, unsigned char *frameBuffer)
// {
//   vec2i bboxmin(WINDOW_WIDTH-1,  WINDOW_HEIGHT-1);
//   vec2i bboxmax(0, 0);
//   vec2i clamp(WINDOW_WIDTH-1, WINDOW_HEIGHT-1);
//   for (int i=0; i<3; i++) {
//       for (int j=0; j<2; j++) {
//           bboxmin[j] = std::max(0,        std::min(bboxmin[j], vertexs[i][j]));
//           bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], vertexs[i][j]));
//       }
//   }
//   vec2i P;
//   for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++)
//   {
//       for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++)
//       {
//           vec3f weight  = barycentric(vertexs, P);
//           if (weight.x<0 || weight.y<0 || weight.z<0) continue;
//           setPixel(P.x, P.y, color, frameBuffer);
//       }
//   }
// }

vec3f barycentric(vec3f *vertexs, vec2f P)
{

  vec3f u = vec3f(vertexs[1][0] - vertexs[0][0], vertexs[2][0] - vertexs[0][0], vertexs[0][0] - P[0]) ^ vec3f(vertexs[1][1] - vertexs[0][1], vertexs[2][1] - vertexs[0][1], vertexs[0][1] - P[1]);
  
  return vec3f(1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
}

static int getIndex(int x, int y)
{
  return (WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x;
}

void rasterizeTriangle(unsigned char *framebuffer, float *zbuffer, IShader &shader)
{
  vec2f bboxmin;
  vec2f bboxmax;
  for (int i = 0; i < 2; i++)
  {
    bboxmin[i] = std::min(shader.payload.screenPos[0][i], std::min(shader.payload.screenPos[1][i], shader.payload.screenPos[2][i]));
    bboxmax[i] = std::max(shader.payload.screenPos[0][i], std::max(shader.payload.screenPos[1][i], shader.payload.screenPos[2][i]));
  }
  int min_x = (int)std::floor(bboxmin[0]);
  int max_x = (int)std::ceil(bboxmax[0]);
  int min_y = (int)std::floor(bboxmin[1]);
  int max_y = (int)std::ceil(bboxmax[1]);
  min_x = std::max(min_x, 0);
  max_x = std::min(max_x, WINDOW_WIDTH - 1);
  min_y = std::max(min_y, 0);
  max_y = std::min(max_y, WINDOW_HEIGHT - 1);
  vec2i p;
  for (p.x = min_x; p.x <= max_x; p.x++)
  {
    for (p.y = min_y; p.y <= max_y; p.y++)
    {
      vec3f weight = barycentric(shader.payload.screenPos, vec2f(p.x+0.5f,p.y+0.5f));
      if (weight.x < 0 || weight.y < 0 || weight.z < 0)
        continue;
      float zp = weight.x * shader.payload.screenPos[0].z + weight.y * shader.payload.screenPos[1].z + weight.z * shader.payload.screenPos[2].z;
      if (zp < zbuffer[getIndex(p.x, p.y)])
      {
        zbuffer[getIndex(p.x, p.y)] = zp;
        Color pixelColor = shader.fragmentShader(weight.x,weight.y,weight.z);
        setPixel(p.x,p.y,pixelColor,framebuffer);
      }
    }
  }
}

void setTriangles(unsigned char *framebuffer, float *zbuffer, IShader &shader, int nface)
{
  shader.vertexShader(nface);
  rasterizeTriangle(framebuffer,zbuffer,shader);
}

#endif