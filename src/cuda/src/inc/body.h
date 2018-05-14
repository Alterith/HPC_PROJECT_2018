/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   octree.h
 * Author: alterith
 *
 * Created on 05 May 2018, 3:47 PM
 */

#ifndef BODY_H
#define BODY_H
#include "dim3float.h"

/*
 * a body struct to store our points
 */

typedef struct body {

    //mass
    double mass;

    //center of mass
	dim3float com;

    //velocity in each direction
	dim3float vel;

} body;

/*
 *  Creates a new body with the given values;
 */

body* create_body(double mass, dim3float com, dim3float vel);



#endif /* BODY_H */

