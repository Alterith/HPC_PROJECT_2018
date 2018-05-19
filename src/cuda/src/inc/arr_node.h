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
    double mass;

    //force
  	double force_x = 0;
    double force_y = 0;
    double force_z = 0;

    //max
  	double max_x = 0;
    double max_y = 0;
    double max_z = 0;

    //min
  	double min_x = 0;
    double min_y = 0;
    double min_z = 0;


    //center of mass
	double com_x = 0;
    double com_y = 0;
    double com_z = 0;


    //velocity in each direction
	double vel_x = 0;
    double vel_y = 0;
    double vel_z = 0;

	//body number
	int body_num;

} arr_node;



#endif /* ARR_NODE_H*/

