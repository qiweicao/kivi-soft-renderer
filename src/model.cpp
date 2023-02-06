#include "model.h"

#include <iostream>
#include <fstream>
#include <sstream>

Model::Model(const char *filename)
{
  std::ifstream in;
  in.open(filename,std::ifstream::in);
  if(in.fail())
  {
    std::cout<<"load model fail"<<std::endl;
    return;
  }
  std::string line;
  while(!in.eof())
  {
    std::getline(in,line);
    std::istringstream iss(line.c_str());
    char trash;
		if (!line.compare(0, 2, "v ")) 
		{
			iss >> trash;
			vec3f v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];
		
			verts.push_back(v);
		}
		else if (!line.compare(0, 3, "vn ")) 
		{
			iss >> trash >> trash;
			vec3f n;
			for (int i = 0; i < 3; i++) 
				iss >> n[i];

			norms.push_back(n);
		}
		else if (!line.compare(0, 3, "vt ")) 
		{
			iss >> trash >> trash;
			vec2f uv;
			
			for (int i = 0; i < 2; i++) 
				iss >> uv[i];
			
			uvs.push_back(uv);
		}
		else if (!line.compare(0, 2, "f ")) 
		{
			std::vector<int> f;
			int tmp[3];
			iss >> trash;
			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) 
			{
				for (int i = 0; i < 3; i++) 
					tmp[i]--; // in wavefront obj all indices start at 1, not zero

				f.push_back(tmp[0]); f.push_back(tmp[1]); f.push_back(tmp[2]);//v,vt,vn
			}
			faces.push_back(f);
		}
  }
  std::cerr << "# v# " << verts.size() << " f# " << faces.size() << " vt# " << uvs.size() << " vn# " << norms.size() << std::endl;
}

int Model::nverts()
{
  return (int)verts.size();
}

int Model::nfaces() 
{
	return (int)faces.size();
}

vec3f Model::vert(int i)
{
  return verts[i];
}

vec3f Model::vert(int nface,int nvert)
{
  return verts[faces[nface][nvert*3]];
}

vec2f Model::uv(int nface,int nvert)
{
  return uvs[faces[nface][nvert*3+1]];
}

vec3f Model::norm(int nface,int nvert)
{
  return norms[faces[nface][nvert*3+2]];
}

std::vector<int> Model::face(int idx) 
{
	std::vector<int> face;
	for (int i = 0; i < 3; i++) 
		face.push_back(faces[idx][i*3]);
	return face;
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) 
{
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) 
	{
		texfile = texfile.substr(0, dot) + std::string(suffix);
		img.read_tga_file(texfile.c_str());
		img.flip_vertically();
	}
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage *img) 
{
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) {
		texfile = texfile.substr(0, dot) + std::string(suffix);
		img->read_tga_file(texfile.c_str());
		img->flip_vertically();
	}
}
void Model::load_texture(const char *filename, TGAImage &img) 
{
  img.read_tga_file(filename);
  img.flip_vertically();
}
void Model::load_texture(const char *filename, TGAImage *img) 
{
  // img=NULL;
  // img= new TGAImage();
  img->read_tga_file(filename);
  img->flip_vertically();
}

vec3f Model::diffuse(vec2f uv)
{
  uv[0]=fmod(uv[0],1);
  uv[1]=fmod(uv[1],1);
  int uv0 = uv[0] * diffuseMap->get_width();
	int uv1 = uv[1] * diffuseMap->get_height();
  TGAColor c = diffuseMap->get(uv0, uv1);
  vec3f res;
  for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f;
  return res;
}

vec3f Model::normal (vec2f uv)
{
  uv[0]=fmod(uv[0],1);
  uv[1]=fmod(uv[1],1);
  int uv0 = uv[0] * normalMap->get_width();
	int uv1 = uv[1] * normalMap->get_height();
  TGAColor c = normalMap->get(uv0, uv1);
  vec3f res;
  for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f*2.f -1.f;
  return res;
}

float Model::specular(vec2f uv)
{
  uv[0]=fmod(uv[0],1);
  uv[1]=fmod(uv[1],1);
  int uv0 = uv[0] * specularMap->get_width();
	int uv1 = uv[1] * specularMap->get_height();
  return specularMap->get(uv0, uv1)[0]/1.f;
}