#ifndef PIPELINE_H
#define PIPELINE_H

#include "linear_algebra.h"
#include "shader.h"

void setPixel(int x, int y, const Color color, unsigned char *frameBuffer);
void setLine(int x0, int y0, int x1, int y1, const Color color, unsigned char *frameBuffer);
void clearFrameBuffer(int width, int height, unsigned char *framebuffer);
void clearZBuffer(int width, int height, float *zbuffer);
vec3f barycentric(vec3f *vertexs, vec2f P);
inline static int getIndex(int x, int y);
void rasterizeTriangle(unsigned char *framebuffer, float *zbuffer, IShader &shader);
void setTriangles(unsigned char *framebuffer, float *zbuffer, IShader &shader, int nface);



#endif
