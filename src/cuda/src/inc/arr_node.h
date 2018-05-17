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

#ifndef ARR_NODE_H
#define ARR_NODE_H
#include "dim3float.h"

/*
 *  octree node struct which contains:
 *  the center of mass,
 *  mass,
 *  the bounds of the cube, 
 *  the 8 children,
 *  the velocity
 *  num of points stored in this branch, 0-8
 *  the force
 *  the body number
 */

typedef struct arr_node {
    //num points
    int furthest_right = -1;

    //mass
    float mass;

    //force
  	float force_x = 0;
    float force_y = 0;
    float force_z = 0;

    //max
  	float max_x = 0;
    float max_y = 0;
    float max_z = 0;

    //min
  	float min_x = 0;
    float min_y = 0;
    float min_z = 0;


    //center of mass
	  float com_x = 0;
    float com_y = 0;
    float com_z = 0;


    //velocity in each direction
	  float vel_x = 0;
    float vel_y = 0;
    float vel_z = 0;

	//body number
	int body_num;

} arr_node;



#endif /* ARR_NODE_H*/

