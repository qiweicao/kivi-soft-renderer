#include "math.hpp"
#include "camera.h"
#include "model.h"
#include "shader.h"


inline vec4f toVec4(vec3f &v,float w)
{
  return vec4f(v.x,v.y,v.z,w);
}

float float_clamp(float f, float min, float max)
{
  return f < min ? min : (f > max ? max : f);
}

void PhongShader::vertexShader(int nface)
{
  // payload.isInCone=0;
  // for(int i=0;i<3;i++)
  // {
  //   vec4f vert = toVec4(payload.model.vert(nface,i),1.0f);
  //   vec4f norm = toVec4(payload.model.norm(nface,i),1.0f);

  // }
  float f1= (Z_FAR -Z_NEAR)/2.0f;
  float f2= (Z_FAR +Z_NEAR)/2.0f;
  vec4f temp;
  for(int i=0;i<3;i++)
  {
    //world coordinate
    payload.outFragPos[i] = payload.model->vert(nface,i);//now model matrix is identity matrix
    payload.outNormal[i] = payload.model->norm(nface,i);
    payload.outTexCoords[i] = payload.model->uv(nface,i);
    //screen coordinate
    temp= payload.mvpMatrix * vec4f(payload.model->vert(nface,i),1.0f);
    payload.screenPos[i]=vec3f(((temp.x/temp.w)+1.0f)*0.5f*WINDOW_WIDTH,
                               ((temp.y/temp.w)+1.0f)*0.5f*WINDOW_HEIGHT,
                               temp.z/temp.w*f1+f2);
  }
}

Color PhongShader::fragmentShader(float alpha, float beta, float gamma)
{
  vec3f ka(0.35,0.35,0.35);
  vec3f kd;
  vec3f ks(0.8,0.8,0.8);
  float p =150.0f;
  vec3f lightPos = vec3f(1.0f,1.0f,1.0f);//point light

  vec3f ambient,diffuse,specular;

  vec3f interpolatedPoint     = interpolate(alpha,beta,gamma,payload.outFragPos[0],  payload.outFragPos[1],  payload.outFragPos[2],  1.0f);
  vec3f interpolatedNormal    = interpolate(alpha,beta,gamma,payload.outNormal[0],   payload.outNormal[1],   payload.outNormal[2],   1.0f).normalize();
  vec2f interpolatedTexCoords = interpolate(alpha,beta,gamma,payload.outTexCoords[0],payload.outTexCoords[1],payload.outTexCoords[2],1.0f);


  kd=payload.model->diffuse(interpolatedTexCoords);
  vec3f lightAmbientIntensity=kd;
  vec3f lightDiffuseIntensity=vec3f(0.9,0.9,0.9);
  vec3f lightSpeculayIntensity=vec3f(0.15,0.15,0.15);

  //ambient
  ambient = ka.multi(lightAmbientIntensity);
  //diffuse
  vec3f lightDir = (lightPos-interpolatedPoint).normalize();
  vec3f viewDir = (payload.camera->eye - interpolatedPoint).normalize();
  diffuse = kd.multi(lightDiffuseIntensity)*(std::max)(0.0f,interpolatedNormal*lightDir);
  //sepcular
  vec3f halfVec = (lightDir + viewDir).normalize();
  specular = ks.multi(lightSpeculayIntensity)*std::pow((std::max)(0.0f,(interpolatedNormal*halfVec)),p);

  vec3f temp = (ambient+diffuse+specular)*255.0f;
  Color result;
  result.r=(int)(float_clamp(temp.r,0.0f,255.0f));
  result.g=(int)(float_clamp(temp.g,0.0f,255.0f));
  result.b=(int)(float_clamp(temp.b,0.0f,255.0f));

  return result;
}
