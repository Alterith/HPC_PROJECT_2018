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
//#include "body.h"

node* malloc_node(float x_1, float y_1, float z_1, float x_2, float y_2, float z_2) {
    //create node to be returned
    node* octree_node = (node*) malloc(sizeof (node));
    //node* octree_node = new node;


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

	return octree_node;

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

int child_node(node* octree_node, dim3float pos, dim3float mid);

/*
 * insert node into tree
 *  mass
 *  position
 *  velocity
 * returns number of nodes in branch
 */

int insert_node(node* octree_node, body *b, int body_num) {
	//placeholder
	//dim3float pos = dim3float(pos_x, pos_y, pos_z);
	//std::cout<<body_num<<std::endl;
    //empty node
    if (octree_node->num_points == 0) {
        octree_node->mass = b->mass;
        octree_node->com = b->com;
        octree_node->vel = b->vel;
		octree_node->body_num = body_num;
        octree_node->num_points++;

    }//node with at least 1 point
    else {

        //move node curr val to child node
        if (octree_node->num_points == 1) {
            int child_num = child_node(octree_node, octree_node->com, octree_node->mid);
            insert_node(octree_node->children[child_num], create_body(octree_node->mass, octree_node->com, octree_node->vel), octree_node->body_num);
        }
        //insert new element into child node
        int child_num = child_node(octree_node, b->com, octree_node->mid);
        insert_node(octree_node->children[child_num], b, body_num);

        //update the node com and velocity according to a weighted average according to their masses and velocities
        //com
        /*
        octree_node->com.x = ((octree_node->com.x * octree_node->mass)+(pos.x * mass)) / (octree_node->mass + mass);
        octree_node->com.y = ((octree_node->com.y * octree_node->mass)+(pos.y * mass)) / (octree_node->mass + mass);
        octree_node->com.z = ((octree_node->com.z * octree_node->mass)+(pos.z * mass)) / (octree_node->mass + mass);
        */

        octree_node->com = (octree_node->com | octree_node->mass)*(1.0/ (octree_node->mass + b->mass));

        //velocities
        octree_node->vel = {0};

        //mass
        octree_node->mass += b->mass;

        // place the values in the appropriate child node

        octree_node->num_points++;
    }

    return octree_node->num_points;
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

