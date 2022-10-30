#include "win32.h"

#include <cassert>

TCHAR szAppName[] = TEXT("renderWindow");
window_t *window = NULL;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void dispatchMsg(void);

static void registerWindowClass()
{
  WNDCLASS wc;
  wc.style = CS_BYTEALIGNCLIENT;                          //窗口风格
  wc.lpfnWndProc = (WNDPROC)WndProc;                      //回调函数
  wc.cbClsExtra = 0;                                      //紧跟在窗口类尾部的一块额外空间，不用则设为0
  wc.cbWndExtra = 0;                                      //紧跟在窗口实例尾部的一块额外空间，不用则设为0
  wc.hInstance = GetModuleHandle(NULL);                   //当前实例句柄
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);             //任务栏图标
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);               //光标样式
  wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景样式
  wc.lpszMenuName = NULL;                                 //菜单
  wc.lpszClassName = szAppName;                           //该窗口类的名字

  ATOM atom = RegisterClass(&wc); //注册窗口类
  assert(atom != 0);
}

int initWindow(int width, int height, const char *title)
{
  window = (window_t *)malloc(sizeof(window_t));
  memset(window, 0, sizeof(window_t));
  window->isClose = 0;
  window->width=width;
  window->height=height;

  //注册窗口类
  registerWindowClass();

  //创建窗口
  window->h_window = CreateWindow(
      szAppName,
      TEXT("RenderWindow"),
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      0,
      0,
      0,
      0,
      NULL,
      NULL,
      GetModuleHandle(NULL),
      NULL);
  assert(window->h_window != NULL);

  //获得辅助DC
  HDC hDc; //系统绘图设备
  hDc = GetDC(window->h_window);
  window->mem_dc = CreateCompatibleDC(hDc); //辅助绘图DC
  ReleaseDC(window->h_window, hDc);

  //创建DIB
  LPVOID ptr;
  BITMAPINFO bi = {{sizeof(BITMAPINFOHEADER),
                    width, -height, 1, 32, BI_RGB,
                    static_cast<DWORD>(width * height * 4), 0, 0, 0, 0}};

  window->bm_dib = CreateDIBSection(window->mem_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
  assert(window->bm_dib != NULL);

  window->bm_old = (HBITMAP)SelectObject(window->mem_dc, window->bm_dib); //把新创建的位图句柄写入mem_dc
  window->window_fb = (unsigned char *)ptr;

  RECT rect = {0, 0, width, height};
  int wx, wy, sx, sy;
  AdjustWindowRect(&rect, GetWindowLong(window->h_window, GWL_STYLE), 0); //调整窗口大小
  wx = rect.right - rect.left;
  wy = rect.bottom - rect.top;
  sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2; // GetSystemMetrics(SM_CXSCREEN)获取你屏幕的分片率
  sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2; // 计算出中心位置
  if (sy < 0)
    sy = 0;

  SetWindowPos(window->h_window, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
  SetForegroundWindow(window->h_window);
  ShowWindow(window->h_window, SW_NORMAL);

  //消息分派
  dispatchMsg();

  //初始化keys, window_fb全为0
  memset(window->window_fb, 0, width * height * 4);
  memset(window->keys, 0, sizeof(char) * 512);
  return 0;
}

void dispatchMsg()
{
  MSG msg;
  while (1)
  {
    // Peek不阻塞，Get会阻塞，PM_NOREMOVE表示如果有消息不处理（留给接下来的Get处理）
    if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
      break; //没消息就溜，确定有消息再用Get
    if (!GetMessage(&msg, NULL, 0, 0))
      break;

    TranslateMessage(&msg); //转换消息 虚拟按键->字符
    DispatchMessage(&msg);  //传送消息给回调
  }
}

void updateWindow()
{
  // LOGFONT logfont; //改变输出字体
	// ZeroMemory(&logfont, sizeof(LOGFONT));
	// logfont.lfCharSet = ANSI_CHARSET;
	// logfont.lfHeight = 20; //设置字体的大小
	// HFONT hFont = CreateFontIndirect(&logfont);


  HDC hDC = GetDC(window->h_window);
  // SelectObject(window->mem_dc, hFont);
	// SetTextColor(window->mem_dc, RGB(190, 190, 190));
	// SetBkColor(window->mem_dc, RGB(80, 80, 80));
  // 把辅助DC的数据传到真正的DC上
  BitBlt(hDC, 0, 0, window->width, window->height, window->mem_dc, 0, 0, SRCCOPY);
  ReleaseDC(window->h_window, hDC);
}

void drawWindow(unsigned char *framebuffer)
{
  int i, j;
  for (int i = 0; i < window->height; i++)
  {
    for (int j = 0; j < window->width; j++)
    {
      int index = (i * window->width + j) * 4;
      window->window_fb[index] = framebuffer[index + 2];
      window->window_fb[index + 1] = framebuffer[index + 1];
      window->window_fb[index + 2] = framebuffer[index];
    }
  }
  updateWindow();
}

int destroyWindow()
{
  if (window->mem_dc)
  {
    if (window->bm_old)
    {
      SelectObject(window->mem_dc, window->bm_old); // 写入原来的bitmap，才能释放DC！
      window->bm_old = NULL;
    }
    DeleteDC(window->mem_dc);
    window->mem_dc = NULL;
  }
  if (window->bm_dib)
  {
    DeleteObject(window->bm_dib);
    window->bm_dib = NULL;
  }
  if (window->h_window)
  {
    CloseWindow(window->h_window);
    window->h_window = NULL;
  }

  free(window);
  return 0;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CLOSE:
    window->isClose = 1;
    break;
  case WM_KEYDOWN:
    window->keys[wParam&511] = 1;
    break;
  case WM_KEYUP:
    window->keys[wParam&511] = 0;
    break;
  case WM_LBUTTONDOWN:
    window->buttons[0]=1;
    break;
  case WM_LBUTTONUP:
    window->buttons[0]=0;
    break;
  case WM_RBUTTONDOWN:
    window->buttons[1]=1;
    break;
  case WM_RBUTTONUP:
    window->buttons[1]=0;
    break;

  // 处理退出
  // case WM_DESTROY:
  //   PostQuitMessage(0);
  //   return 0;
  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}

vec2f getMousePos()
{
  POINT point;
  GetCursorPos(&point);
  ScreenToClient(window->h_window,&point);
  return vec2f((float)point.x,(float)point.y);
}
