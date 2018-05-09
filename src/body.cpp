
#include "body.h"

body create_body(double mass, dim3float com, dim3float vel)
{
    body b;
    b.mass = mass;
    b.com = com;
    b.vel = vel;

    return b;
}