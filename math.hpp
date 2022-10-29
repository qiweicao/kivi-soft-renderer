#ifndef MATH_H
#define MATH_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>

#include "global.h"

// 颜色结构
//struct Color{ int r,g,b; Color(int rr,int gg,int bb):r(rr),b(bb),g(gg){} };

template<class T> struct Vector2
{
  union 
  {
    struct{T x,y;};
    struct{T u,v;};
    T raw[2];
  };
  Vector2():x(0),y(0){ }
	Vector2(T xx,T yy):x(xx),y(yy){ }

  T& operator[](const size_t i)       { assert(i<2); return i<=0 ? x : y; }
  const T& operator[](const size_t i) const { assert(i<2); return i<=0 ? x : y; }

	inline Vector2<T> operator+ (const Vector2<T>& a) const{ return Vector2<T>(a.x+x,a.y+y); }
	inline Vector2<T> operator- (const Vector2<T>& a) const{ return Vector2<T>(x-a.x,y-a.y); }
	inline Vector2<T> operator* (T f) const {return Vector2<T>(x*f,y*f); }
};

template <class T> struct Vector3
{
	union {
		struct {T x, y, z;};
    struct {T r, g, b;};
		struct {T ivert, iuv, inorm; };
		T raw[3];
	};
	Vector3() : x(0), y(0), z(0) {}
	Vector3(T xx, T yy, T zz) : x(xx),y(yy),z(zz) {}
	Vector3(T data[3]):x(data[0]),y(data[1]),z(data[2]){}

  T& operator[](const size_t i)       { assert(i<3); return i<=0 ? x : (1==i ? y : z); }
  const T& operator[](const size_t i) const { assert(i<3); return i<=0 ? x : (1==i ? y : z); }

	//叉乘
	inline Vector3<T> operator^ (const Vector3<T> &v) const { return Vector3<T>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vector3<T> operator+ (const Vector3<T> &v) const { return Vector3<T>(x+v.x, y+v.y, z+v.z); }
  inline Vector3<T> operator+=(const Vector3<T> &v) { x=x+v.x; y=y+v.y; z=z+v.z; return *this; }
	inline Vector3<T> operator- (const Vector3<T> &v) const { return Vector3<T>(x-v.x, y-v.y, z-v.z); }
  inline Vector3<T> operator-=(const Vector3<T> &v) { x=x-v.x; y=y-v.y; z=z-v.z; return *this; }
	inline Vector3<T> operator* (T f) const { return Vector3<T>(x*f, y*f, z*f); }
	inline T operator* (const Vector3<T> &v) const { return x*v.x + y*v.y + z*v.z; }
	float length() const { return std::sqrt(x*x+y*y+z*z); }
	Vector3<T> normalize() { return (*this)*(1/length());  }
  Vector3<T> multi (const Vector3<T> &v) const { return Vector3<T>(x*v.x, y*v.y, z*v.z); }
};

struct Vector4f
{
  union
  {
    struct{ float x, y, z, w;};
    float raw[4];
  };
  Vector4f() {}
  Vector4f(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {}
  Vector4f(const Vector3<float> &v, float ww) : x(v.x),y(v.y),z(v.z),w(ww) {}
  Vector4f(const std::vector<float> &v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}

  float& operator[](const size_t i)       { assert(i<4); return i<=0 ? x : raw[i]; }
  const float& operator[](const size_t i) const { assert(i<4); return i<=0 ? x : raw[i]; }

  Vector4f operator+(const Vector4f &v) const { return Vector4f(x + v.x, y + v.y, z + v.z, w+v.w); }
  Vector4f operator-(const Vector4f &v) const { return Vector4f(x - v.x, y - v.y, z - v.z, w-v.w); }
  Vector4f operator*(float f) const { return Vector4f(x * f, y * f, z * f, w * f); }
  float operator*(const Vector4f &v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
  Vector4f operator/(float f) const { return Vector4f(x / f, y / f, z / f, w / f); }

  //长度
  float length() const { return (float)sqrt(x * x + y * y + z * z + w * w); }
  //归一化
  Vector4f normalize() const
  {
    float length = this->length();
    float inv = 1.0f / length;
    return Vector4f(x * inv, y * inv, z * inv, 1.0f);
  }
};

class Matrix4f
{
private:
  std::vector<std::vector<float>> m;
  int raws, cols;

public:
  //默认4*4单位矩阵
  Matrix4f(float f = 1.0f) : m(std::vector<std::vector<float>>(4, std::vector<float>(4, 0.0f))), cols(4), raws(4)
  {
    m[0][0] = f;
    m[1][1] = f;
    m[2][2] = f;
    m[3][3] = f;
  }
  Matrix4f(const std::vector<std::vector<float>> &mat) : raws(4), cols(4)
  {
    int i, j;
    for (i = 0; i < raws; i++)
    {
      for (j = 0; j < cols; j++)
      {
        m[i][j] = mat[i][j];
      }
    }
  }

  //+
  Matrix4f operator+(const Matrix4f &mat) const
  {
    int i, j;
    Matrix4f res(0.0f);
    for (i = 0; i < raws; i++)
    {
      for (j = 0; j < cols; j++)
      {
        res.m[i][j] = m[i][j] + mat.m[i][j];
      }
    }
    return res;
  }

  //-
  Matrix4f operator-(const Matrix4f &mat) const
  {
    int i, j;
    Matrix4f res(0.0f);
    for (i = 0; i < raws; i++)
    {
      for (j = 0; j < cols; j++)
      {
        res.m[i][j] = m[i][j] - mat.m[i][j];
      }
    }
    return res;
  }

  //*
  Matrix4f operator*(const Matrix4f &mat) const
  {
    int i, j;
    Matrix4f res(0.0f);
    for (i = 0; i < raws; i++)
    {
      for (j = 0; j < cols; j++)
      {
        res.m[i][j] = m[i][0] * mat.m[0][j] +
                      m[i][1] * mat.m[1][j] +
                      m[i][2] * mat.m[2][j] +
                      m[i][3] * mat.m[3][j];
      }
    }
    return res;
  }

  Vector4f operator*(const Vector4f &v) const
  {
    float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
    return Vector4f(x, y, z, w);
  }

private:
  class MatrixLoader
  {
  public:
    MatrixLoader(Matrix4f& M, int i):M(M),i(i){}
    MatrixLoader operator,(const float &val)
    {
      M.m[i/4][i%4] = val;
      return MatrixLoader(M,i+1);
    }
    private:  
      Matrix4f &M;
      int i;
  };
public:
  MatrixLoader operator<<(const float &val)
  {
    
    m[0][0] = val;
    return MatrixLoader(*this,1);
  }

  friend std::ostream& operator<<(std::ostream& out, Matrix4f& mat)
  {
    for (auto v:mat.m)
    {
      for(auto f:v)
      {
        out<<"\t"<<f;
      }
      out<<std::endl;
    }
    return out;
  }

};


typedef Vector2<float>  vec2f;
typedef Vector2<int>    vec2i;
typedef Vector3<float>  vec3f;
typedef Vector3<int>    vec3i;
typedef Vector3<int>    Color;
typedef Vector4f        vec4f;
typedef Matrix4f        mat4f;


#endif