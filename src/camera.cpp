#include "camera.h"

#include "linear_algebra.h"
#include "win32.h"


Camera::Camera(vec3f eye, vec3f target, vec3f up, float aspect):
  eye(eye),target(target),up(up),aspect(aspect) {yaw=YAW;pitch=PITCH;};

void processInput(Camera &camera)
{
  //mat4f view;
  camera.z=(camera.eye-camera.target).normalize();
  camera.x=(camera.up^camera.z).normalize();
  camera.y=(camera.z^camera.x).normalize();


  float sensitivity =SENSITIVITY;
  vec2f curPos=getMousePos();
  window->mouseInfo.posOffset = curPos-window->mouseInfo.pos;
  window->mouseInfo.pos = curPos;

  float xoffset=window->mouseInfo.posOffset.x*sensitivity;
  float yoffset=window->mouseInfo.posOffset.y*sensitivity;

  camera.yaw += xoffset;
  camera.pitch -= yoffset;
  if(camera.pitch>89.0f) camera.pitch=89.0f;
  if(camera.pitch<-89.0f) camera.pitch=-89.0f;

  vec3f front;
  front.x = cos(camera.yaw/180.0f*PI)*cos(camera.pitch/180.0f*PI);
  front.y = sin(camera.pitch/180.0f*PI);
  front.z = sin(camera.yaw/180.0f*PI)*cos(camera.pitch/180.0f*PI);
  front=front.normalize();
  camera.target=camera.eye+front;

  float cameraSpeed = SPEED;

  if(window->keys['W'])
  {
    camera.eye += camera.z*(-cameraSpeed);
  }
  if(window->keys['S'])
  {
    camera.eye += camera.z*cameraSpeed;
  }
  if(window->keys['Q'])
  {
    camera.eye += camera.y*cameraSpeed;
    camera.target += camera.y*cameraSpeed;
  }
  if(window->keys['E'])
  {
    camera.eye -= camera.y*cameraSpeed;
    camera.target -= camera.y*cameraSpeed;
  }
  if(window->keys['A'])
  {
    camera.eye -= camera.x*cameraSpeed;
    camera.target -= camera.x*cameraSpeed;
  }
  if(window->keys['D'])
  {
    camera.eye += camera.x*cameraSpeed;
    camera.target += camera.x*cameraSpeed;
  }
  if(window->keys[VK_ESCAPE])
  {
    window->isClose = 1;
  }

}

mat4f getModelMatrix()
{
  return mat4f();//暂时是单位矩阵
}

mat4f getViewMatrix(vec3f eye,vec3f target, vec3f up)
{
  mat4f view;
  vec3f z=(eye-target).normalize();
  vec3f x=(up^z).normalize();
  vec3f y=(z^x).normalize();

  view<<x[0],x[1],x[2],-(x*eye),
        y[0],y[1],y[2],-(y*eye),
        z[0],z[1],z[2],-(z*eye),
        0.0f,0.0f,0.0f,1.0f;

  return view;
}

mat4f getProjectMatrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
  mat4f projection=mat4f();
  float r,l,t,b;
  float n=zNear;
  float f=zFar;
  t = std::tan(eye_fov/2.0/180.0*PI) * std::abs(zNear);
  r = aspect_ratio * t;
  b = -t;
  l = -r;
  mat4f translatePersp, translateOrtho, translate;
  translatePersp << n, 0.0, 0.0, 0.0,
                    0.0, n, 0.0, 0.0,
                    0.0, 0.0, n+f, -n*f,
                    0.0, 0.0, 1.0, 0.0;
  translate << 1.0, 0.0, 0.0, -(r+l)/2.0,
               0.0, 1.0, 0.0, -(t+b)/2.0,
               0.0, 0.0, 1.0, -(n+f)/2.0,
               0.0, 0.0, 0.0, 1.0;
  translateOrtho << 2.0/(r-l), 0.0, 0.0, 0.0,
                    0.0, 2.0/(t-b), 0.0, 0.0,
                    0.0, 0.0, 2.0/(n-f), 0.0,
                    0.0, 0.0, 0.0, 1.0;
  translateOrtho = translateOrtho * translate;

  projection = translateOrtho * translatePersp;
  // projection << zNear/r,0.0,0.0,0.0,
  //               0.0,zNear/t,0.0,0.0,
  //               0.0,0.0,(zNear+zFar)/(zNear-zFar),2*zNear*zFar/(zFar-zNear),
  //               0.0,0.0,1.0,0.0;
  return projection;
}