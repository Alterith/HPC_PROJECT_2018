
#include "body.h"

body *create_body(double mass, dim3float com, dim3float vel)
{
    body *b = (body *)malloc(sizeof(body));
    b->mass = mass;
    b->com = com;
    b->vel = vel;

    return b;
}