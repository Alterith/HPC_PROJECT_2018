#ifndef DIM3FLOAT_H
#define DIM3FLOAT_H

#include <cmath>
#include <iostream>
#include <iomanip>

// Based on dim3 by Nvidia for use in CUDA, defined in "vector_types.h", adjusted for use with floats
//----
struct dim3float
{
  double x, y, z;
  dim3float(double vx = 0, double vy = 0, double vz = 0) : x(vx), y(vy), z(vz) {}

  void cout2()
  {
    std::cout << std::fixed;
    std::cout << std::setw(11) << std::setprecision(4) << x << std::setw(11) << std::setprecision(4) << y << std::setw(11) << std::setprecision(4) << z << std::endl;
  }

  void new_pos(const dim3float &vel, const double delta_t)
  {
    x = x + vel.x * delta_t;
    y = y + vel.y * delta_t;
    z = z + vel.z * delta_t;
  }

  //Element wise addition dim3float .+ dim3float
  dim3float operator+(const dim3float &a)
  {
    double vx = x + a.x;
    double vy = y + a.y;
    double vz = z + a.z;
    return dim3float(vx, vy, vz);
  }

  //Element wise subtraction dim3float .- dim3float
  dim3float operator-(const dim3float &a)
  {
    double vx = x - a.x;
    double vy = y - a.y;
    double vz = z - a.z;
    return dim3float(vx, vy, vz);
  }

  //Element wise multiplication dim3float .* dim3float
  dim3float operator*(const dim3float &a)
  {
    double vx = x * a.x;
    double vy = y * a.y;
    double vz = z * a.z;
    return dim3float(vx, vy, vz);
  }
  //Element wise power dim3float .^ int
  dim3float operator^(const int a)
  {
    double vx = pow(x, a);
    double vy = pow(y, a);
    double vz = pow(z, a);
    return dim3float(vx, vy, vz);
  }

  //Constant multiplication dim3float .* constant
  dim3float operator|(const float a)
  {
    double vx = x * a;
    double vy = y * a;
    double vz = z * a;
    return dim3float(vx, vy, vz);
  }
};

typedef struct dim3float dim3float;

//----

#endif /* DIM3FLOAT */
