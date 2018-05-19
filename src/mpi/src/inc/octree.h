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
#include "body.h"

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

typedef struct node {
    //num points
    int num_points;

    // 8 children pointers
    struct node* children[8];

    //mass
    float mass;

    //force
  	dim3float force;


    //center of mass
	dim3float com;


    //bounds: used for splitting into 8 branches

    //min bounds
    dim3float min;

    //max bounds
    dim3float max;

    //mid point
    dim3float mid;

    //velocity in each direction
	dim3float vel;

	//body number
	int body_num;

} node;

/*
 *  Mallocs the relevant values for the new node
 *  no new point is assigned by this statement just the cube bounds, and default values
 */

node* malloc_node(float x_1, float y_1, float z_1, float x_2, float y_2, float z_2);

/*
 * free memory taken by node and children
 */

void free_node(node* octree_node);

/* Pass mid, max or min, mid into the function
 * insert node into tree
 *  mass
 *  position
 *  velocity
 * returns success code 0, fail 1
 */

int insert_node(node* octree_node, body b, int body_num);


#endif /* OCTREE_H */

