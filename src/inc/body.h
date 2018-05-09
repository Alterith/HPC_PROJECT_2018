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

#ifndef OCTREE_H
#define OCTREE_H
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
 *  Mallocs the relevant values for the new node
 *  no new point is assigned by this statement just the cube bounds, and default values
 */

body* malloc_body(double x_1, double y_1, double z_1, double x_2, double y_2, double z_2);



#endif /* OCTREE_H */

