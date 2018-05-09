/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 *  Mallocs the relevant values for the new node
 *  no new point is assigned by this statement just the cube bounds, and default values
 */
#include <stdlib.h> 
#include <iostream>
#include "octree.h"

node* malloc_node(float x_1, float y_1, float z_1, float x_2, float y_2, float z_2) {
    //create node to be returned
    node* octree_node = (node*) malloc(sizeof (node));

    //initialize children to null
    for (int i = 0; i < 8; i++) {
        octree_node->children[i] = nullptr;
    }
	

    //set mass, num_points, force, com and velocity to 0
    octree_node->mass = 0;
	
	octree_node->body_num = 0;	
	
    octree_node->num_points = 0;

    octree_node->force = dim3float(0, 0, 0);

    octree_node->com = dim3float(0, 0, 0);

    octree_node->vel = dim3float(0, 0, 0);
	
	octree_node->min = dim3float(0, 0, 0);

	octree_node->max = dim3float(0, 0, 0);

	octree_node->mid = dim3float(0, 0, 0);

    //assign bounds values

    if (x_1 < x_2) {
        octree_node->min.x = x_1;
        octree_node->max.x = x_2;
    } else {
        octree_node->min.x = x_2;
        octree_node->max.x = x_1;
    }

    if (y_1 < y_2) {
        octree_node->min.y = y_1;
        octree_node->max.y = y_2;
    } else {
        octree_node->min.y = y_2;
        octree_node->max.y = y_1;
    }

    if (z_1 < z_2) {
        octree_node->min.z = z_1;
        octree_node->max.z = z_2;
    } else {
        octree_node->min.z = z_2;
        octree_node->max.z = z_1;
    }

    // assign mid point coordinates
    octree_node->mid.x = (float) ((x_1 + x_2) / 2);
    octree_node->mid.y = (float) ((y_1 + y_2) / 2);
    octree_node->mid.z = (float) ((z_1 + z_2) / 2);

}

/*
 * free memory taken by node and children
 */

void free_node(node* octree_node) {
    //free children
    for (int i = 0; i < 8; i++) {
        if(octree_node->children[i])
            free_node(octree_node->children[i]);
    }
    //free node
    free(octree_node);

}

int child_node(node* octree, dim3float pos, dim3float mid);

/*
 * insert node into tree
 *  mass
 *  position
 *  velocity
 * returns number of nodes in branch
 */

int insert_node(node* octree, float mass, float pos_x, float pos_y, float pos_z, float vel_x, float vel_y, float vel_z, int body_num) {
	//placeholder
	dim3float pos = dim3float(pos_x, pos_y, pos_z);
	//std::cout<<body_num<<std::endl;
    //empty node
    if (octree->num_points == 0) {
        octree->mass = mass;
        octree->com.x = pos_x;
        octree->com.y = pos_y;
        octree->com.z = pos_z;
        octree->vel.x = vel_x;
        octree->vel.y = vel_y;
        octree->vel.z = vel_z;
		octree->body_num = body_num;
        octree->num_points++;

    }//node with at least 1 point
    else {

        //move node curr val to child node
        if (octree->num_points == 1) {
            int child_num = child_node(octree, octree->com, octree->mid);
            insert_node(octree->children[child_num], octree->mass, octree->com.x, octree->com.y, octree->com.z, octree->vel.x, octree->vel.y, octree->vel.z, octree->body_num);
        }
        //insert new element into child node
        int child_num = child_node(octree, pos, octree->mid);
        insert_node(octree->children[child_num], mass, pos.x, pos.y, pos.z, vel_x, vel_y, vel_z, body_num);

        //update the node com and velocity according to a weighted average according to their masses and velocities
        //com
        octree->com.x = ((octree->com.x * octree->mass)+(pos.x * mass)) / (octree->mass + mass);
        octree->com.y = ((octree->com.y * octree->mass)+(pos.y * mass)) / (octree->mass + mass);
        octree->com.z = ((octree->com.z * octree->mass)+(pos.z * mass)) / (octree->mass + mass);

        //velocities
        octree->vel.x = 0;
        octree->vel.y = 0;
        octree->vel.z = 0;

        //mass
        octree->mass += mass;

        // place the values in the appropriate child node

        octree->num_points++;
    }

    return octree->num_points;
}

int child_node(node* octree, dim3float pos, dim3float mid) {

    int child = -1;
    //check bounds and return appropriate child num
    if (pos.x <= mid.x) {
        if (pos.y <= mid.y) {
            if (pos.z <= mid.z) {
                child = 2;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid.x, mid.y, mid.z, octree->min.x, octree->min.y, octree->min.z);
                }
            } else {
                child = 6;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid.x, mid.y, mid.z, octree->min.x, octree->min.y, octree->max.z);
                }
            }
        } else {
            if (pos.z <= mid.z) {
                child = 0;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid.x, mid.y, mid.z, octree->min.x, octree->max.y, octree->min.z);
                }
            } else {
                child = 4;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid.x, mid.y, mid.z, octree->min.x, octree->max.y, octree->max.z);
                }
            }
        }
    } else {
        if (pos.y <= mid.y) {
            if (pos.z <= mid.z) {
                child = 3;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid.x, mid.y, mid.z, octree->max.x, octree->min.y, octree->min.z);
                }
            } else {
                child = 7;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid.x, mid.y, mid.z, octree->max.x, octree->min.y, octree->max.z);
                }
            }
        } else {
            if (pos.z <= mid.z) {
                child = 1;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid.x, mid.y, mid.z, octree->max.x, octree->max.y, octree->min.z);
                }
            } else {
                child = 5;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid.x, mid.y, mid.z, octree->max.x, octree->max.y, octree->max.z);
                }
            }
        }
    }
    return child;
}

