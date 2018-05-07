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

/*
 *  octree node struct which contains:
 *  the center of mass,
 *  mass,
 *  the bounds of the cube, 
 *  the 8 children,
 *  the velocity
 *  num of points stored in this branch, 0-8
 *  the force
 */

typedef struct node {
    //num points
    int num_points;

    // 8 children pointers
    struct node* children[8];

    //mass
    double mass;

    //force
    double force_x;
    double force_y;
    double force_z;

    //center of mass
    double com_x;
    double com_y;
    double com_z;

    //bounds: used for splitting into 8 branches

    //min bounds
    double min_x;
    double min_y;
    double min_z;

    //max bounds
    double max_x;
    double max_y;
    double max_z;

    //mid point
    double mid_x;
    double mid_y;
    double mid_z;

    //velocity in each direction
    double vel_x;
    double vel_y;
    double vel_z;
} node;

/*
 *  Mallocs the relevant values for the new node
 *  no new point is assigned by this statement just the cube bounds, and default values
 */

node* malloc_node(double x_1, double y_1, double z_1, double x_2, double y_2, double z_2);

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

int insert_node(node* octree, double mass, double pos_x, double pos_y, double pos_z, double vel_x, double vel_y, double vel_z);



#endif /* OCTREE_H */

