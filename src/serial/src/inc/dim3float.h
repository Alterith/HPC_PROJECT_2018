#ifndef DIM3FLOAT_H
#define DIM3FLOAT_H

#include <cmath>
#include <iostream>
#include <iomanip>

// Based on dim3 by Nvidia for use in CUDA, defined in "vector_types.h", adjusted for use with floats
//----
struct dim3float
{
  float x, y, z;
  dim3float(float vx = 0, float vy = 0, float vz = 0) : x(vx), y(vy), z(vz) {}

  void cout2()
  {
    std::cout << std::fixed;
    std::cout << std::setw(15) << std::setprecision(4) << x << std::setw(15) << std::setprecision(4) << y << std::setw(15) << std::setprecision(4) << z << std::endl;
  }

  void new_pos(const dim3float &vel, const float delta_t, int bound)
  {
    x = x + vel.x * delta_t;
	while(abs(x)>bound){
		double rem = 0;
		if(x > bound){
			rem = x - bound;
			x = (-1)*bound + rem;
		}else if(x < (-1)*bound){
			rem = abs(x)-abs(bound);
			x = bound - rem;
		}
		//std::cout<<rem<<std::endl;
	}
	
    y = y + vel.y * delta_t;
	while(abs(y)>bound){
		double rem = 0;
		if(y > bound){
			rem = y - bound;
			y = (-1)*bound + rem;
		}else if(y < (-1)*bound){
			rem = abs(y)-abs(bound);
			y = bound - rem;
		}
		//std::cout<<rem<<std::endl;
	}

    z = z + vel.z * delta_t;
	while(abs(z)>bound){
		double rem = 0;
		if(z > bound){
			rem = z - bound;
			z = (-1)*bound + rem;
		}else if(z < (-1)*bound){
			rem = abs(z)-abs(bound);
			z = bound - rem;
		}
		//std::cout<<rem<<std::endl;
	}
  }

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

#endif /* DIM3FLOAT */
