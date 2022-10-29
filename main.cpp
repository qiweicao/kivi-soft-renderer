#include <iostream>
#include <vector>

#include "win32.h"
#include "math.hpp"
#include "pipeline.hpp"
#include "model.h"
#include "camera.h"
#include "shader.hpp"
#include "tgaimage.h"

const vec3f Eye(0, 1, 1);
const vec3f Up(0, 1, 0);
const vec3f Target(0, 1, -1);

int main(int, char **)
{
  int width=WINDOW_WIDTH,height=WINDOW_HEIGHT;
  unsigned char *framebuffer  = (unsigned char *)malloc(sizeof(unsigned char) * width * height * 4);
  memset(framebuffer, 0, sizeof(unsigned char) * width * height * 4);
  float *zbuffer = new float[width*height];
  //for (int i=width*height; i--; zbuffer[i] = std::numeric_limits<float>::max());

  Model model("../obj/african_head/african_head.obj");
  model.diffuseMap=NULL;
  model.diffuseMap= new TGAImage();
  model.load_texture("../obj/african_head/african_head_diffuse.tga",model.diffuseMap);
  // Model model("../obj/cube.obj");
  // model.diffuseMap=NULL;
  // model.diffuseMap= new TGAImage();
  // model.load_texture("../obj/awesomeface.tga",model.diffuseMap);

  Camera camera(Eye,Target,Up,((float)width/height));

  IShader *shaderOfModel = new PhongShader();
  shaderOfModel->payload.camera = &camera;
  shaderOfModel->payload.model = &model;
  initWindow(width, height, "RenderWindow");

  // mat4f m;
  // m<<1,2,3,4,
  //    5,6,7,8,
  //    9,10,11,12,
  //    13,14,15,16;
  // std::cout<<m;

  //std::vector<vec2i> vertexs{vec2i(200,200),vec2i(400,300),vec2i(200,600)};
  //setTriangle(vertexs,Color(255,0,0),framebuffer);
  while (!window->isClose)
  {
    
    dispatchMsg();
    clearFrameBuffer(width,height,framebuffer);
    clearZBuffer(width,height,zbuffer);
    processInput(camera);

    shaderOfModel->payload.modelMatrix=getModelMatrix();
    shaderOfModel->payload.viewMatrix=getViewMatrix(camera.eye,camera.target,camera.up);
    shaderOfModel->payload.projectMatrix=getProjectMatrix(60.0f,((float)width/height),Z_NEAR,Z_FAR);
    shaderOfModel->payload.mvpMatrix=shaderOfModel->payload.projectMatrix*shaderOfModel->payload.viewMatrix*shaderOfModel->payload.modelMatrix;

    
    for(int i=0;i<model.nfaces();i++)
    {
      setTriangles(framebuffer,zbuffer,*shaderOfModel,i);
    }
    // setTriangles(framebuffer,zbuffer,*shaderOfModel,0);
    // setTriangles(framebuffer,zbuffer,*shaderOfModel,1);
    
    drawWindow(framebuffer);
    //Sleep(1);
  }
  free(shaderOfModel);
  free(zbuffer);
  free(framebuffer);
  destroyWindow();
  return 0;
}
