#ifndef WIN32_H
#define WIN32_H

#include <Windows.h>
#include "math.hpp"

typedef struct mouse
{
	vec2f pos;
	vec2f posOffset;
	float wheelOffset;
}mouse_t;

typedef struct window
{
  HWND h_window;
  HDC mem_dc;
  HBITMAP bm_dib;
  HBITMAP bm_old;
  unsigned char *window_fb;
  int width;
  int height;
  char keys[512];
  char buttons[2];
  int isClose;
  mouse_t mouseInfo;
} window_t;

extern window_t *window;

void dispatchMsg();
int initWindow(int width, int height, const char *title);
void drawWindow(unsigned char *framebuffer);
int destroyWindow();
vec2f getMousePos();

#endif