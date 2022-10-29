#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "math.hpp"
#include "tgaimage.h"

class Model
{
public:
  Model(const char *filename);

  TGAImage *diffuseMap;
  TGAImage *normalMap;
  TGAImage *specularMap;
  TGAImage *roughnessMap;
  TGAImage *glossMap;

  void load_texture(std::string filename, const char *suffix, TGAImage &img);
	void load_texture(std::string filename, const char *suffix, TGAImage *img);
  void load_texture(const char *filename, TGAImage &img);
  void load_texture(const char *filename, TGAImage *img);

  int nverts();
  int nfaces();

  vec3f vert(int i);
  vec3f vert(int nface,int nvert);
  vec3f norm(int i);
  vec3f norm(int nface,int nvert);
  std::vector<int> face(int idx);//9*int,3*(v,vt,vn)
  vec2f uv(int nface,int nvert);

  vec3f diffuse(vec2f uv);
  vec3f normal (vec2f uv);
  float specular(vec2f uv);


private:
  std::vector<vec3f> verts;
  std::vector<std::vector<int>> faces;
  std::vector<vec3f> norms;
  std::vector<vec2f> uvs;

  
};

#endif