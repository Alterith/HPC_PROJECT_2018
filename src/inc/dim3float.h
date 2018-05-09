#include <cmath>

// Based on dim3 by Nvidia for use in CUDA, defined in "vector_types.h", adjusted for use with floats
//----
struct dim3float
{
  float x, y, z;
  dim3float(float vx = 0, float vy = 0, float vz = 0) : x(vx), y(vy), z(vz) {}

  //Element wise addition dim3float .+ dim3float
  dim3float operator+(const dim3float &a)
  {
    float vx = x + a.x;
    float vy = y + a.y;
    float vz = z + a.z;
    return dim3float(vx, vy, vz);
  }

  //Element wise subtraction dim3float .- dim3float
  dim3float operator-(const dim3float &a)
  {
    float vx = x - a.x;
    float vy = y - a.y;
    float vz = z - a.z;
    return dim3float(vx, vy, vz);
  }

  //Element wise multiplication dim3float .* dim3float
  dim3float operator*(const dim3float &a)
  {
    float vx = x * a.x;
    float vy = y * a.y;
    float vz = z * a.z;
    return dim3float(vx, vy, vz);
  }
  //Element wise power dim3float .^ int
  dim3float operator^(const int a)
  {
    float vx = pow(x, a);
    float vy = pow(y, a);
    float vz = pow(z, a);
    return dim3float(vx, vy, vz);
  }

  //Constant multiplication dim3float .* constant
  dim3float operator|(const float a)
  {
    float vx = x * a;
    float vy = y * a;
    float vz = z * a;
    return dim3float(vx, vy, vz);
  }
};

typedef struct dim3float dim3float;

//----
