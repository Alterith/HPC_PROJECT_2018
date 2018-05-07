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
#include "octree.h"

node* malloc_node(double x_1, double y_1, double z_1, double x_2, double y_2, double z_2) {
    //create node to be returned
    node* octree_node = (node*) malloc(sizeof (node));

    //initialize children to null
    for (int i = 0; i < 8; i++) {
        octree_node->children[i] = nullptr;
    }

    //set mass, num_points, force, com and velocity to 0
    octree_node->mass = 0;
    octree_node->num_points = 0;

    octree_node->force_x = 0;
    octree_node->force_y = 0;
    octree_node->force_z = 0;

    octree_node->com_x = 0;
    octree_node->com_y = 0;
    octree_node->com_z = 0;

    octree_node->vel_x = 0;
    octree_node->vel_y = 0;
    octree_node->vel_z = 0;

    //assign bounds values

    if (x_1 < x_2) {
        octree_node->min_x = x_1;
        octree_node->max_x = x_2;
    } else {
        octree_node->min_x = x_2;
        octree_node->max_x = x_1;
    }

    if (y_1 < y_2) {
        octree_node->min_y = y_1;
        octree_node->max_y = y_2;
    } else {
        octree_node->min_y = y_2;
        octree_node->max_y = y_1;
    }

    if (z_1 < z_2) {
        octree_node->min_z = z_1;
        octree_node->max_z = z_2;
    } else {
        octree_node->min_z = z_2;
        octree_node->max_z = z_1;
    }

    // assign mid point coordinates
    octree_node->mid_x = (double) ((x_1 + x_2) / 2);
    octree_node->mid_y = (double) ((y_1 + y_2) / 2);
    octree_node->mid_z = (double) ((z_1 + z_2) / 2);

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

int child_node(node* octree, double pos_x, double pos_y, double pos_z, double mid_x, double mid_y, double mid_z);

/*
 * insert node into tree
 *  mass
 *  position
 *  velocity
 * returns number of nodes in branch
 */

int insert_node(node* octree, double mass, double pos_x, double pos_y, double pos_z, double vel_x, double vel_y, double vel_z) {
    //empty node
    if (octree->num_points == 0) {
        octree->mass = mass;
        octree->com_x = pos_x;
        octree->com_y = pos_y;
        octree->com_z = pos_z;
        octree->vel_x = vel_x;
        octree->vel_y = vel_y;
        octree->vel_z = vel_z;
        octree->num_points++;

    }//node with at least 1 point
    else {

        //move node curr val to child node
        if (octree->num_points == 1) {
            int child_num = child_node(octree, octree->com_x, octree->com_y, octree->com_z, octree->mid_x, octree->mid_y, octree->mid_z);
            insert_node(octree->children[child_num], octree->mass, octree->com_x, octree->com_y, octree->com_z, octree->vel_x, octree->vel_y, octree->vel_z);
        }
        //insert new element into child node
        int child_num = child_node(octree, pos_x, pos_y, pos_z, octree->mid_x, octree->mid_y, octree->mid_z);
        insert_node(octree->children[child_num], mass, pos_x, pos_y, pos_z, vel_x, vel_y, vel_z);

        //update the node com and velocity according to a weighted average according to their masses and velocities
        //com
        octree->com_x = ((octree->com_x * octree->mass)+(pos_x * mass)) / (octree->mass + mass);
        octree->com_y = ((octree->com_y * octree->mass)+(pos_y * mass)) / (octree->mass + mass);
        octree->com_z = ((octree->com_z * octree->mass)+(pos_z * mass)) / (octree->mass + mass);

        //velocities
        octree->vel_x = 0;
        octree->vel_y = 0;
        octree->vel_z = 0;

        //mass
        octree->mass += mass;

        // place the values in the appropriate child node

        octree->num_points++;
    }

    return octree->num_points;
}

int child_node(node* octree, double pos_x, double pos_y, double pos_z, double mid_x, double mid_y, double mid_z) {

    int child = -1;
    //check bounds and return appropriate child num
    if (pos_x <= mid_x) {
        if (pos_y <= mid_y) {
            if (pos_z <= mid_z) {
                child = 2;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid_x, mid_y, mid_z, octree->min_x, octree->min_y, octree->min_z);
                }
            } else {
                child = 6;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid_x, mid_y, mid_z, octree->min_x, octree->min_y, octree->max_z);
                }
            }
        } else {
            if (pos_z <= mid_z) {
                child = 0;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid_x, mid_y, mid_z, octree->min_x, octree->max_y, octree->min_z);
                }
            } else {
                child = 4;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid_x, mid_y, mid_z, octree->min_x, octree->max_y, octree->max_z);
                }
            }
        }
    } else {
        if (pos_y <= mid_y) {
            if (pos_z <= mid_z) {
                child = 3;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid_x, mid_y, mid_z, octree->max_x, octree->min_y, octree->min_z);
                }
            } else {
                child = 7;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid_x, mid_y, mid_z, octree->max_x, octree->min_y, octree->max_z);
                }
            }
        } else {
            if (pos_z <= mid_z) {
                child = 1;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid_x, mid_y, mid_z, octree->max_x, octree->max_y, octree->min_z);
                }
            } else {
                child = 5;
                //create child if doesn't exist
                if (!octree->children[child]) {
                    octree->children[child] = malloc_node(mid_x, mid_y, mid_z, octree->max_x, octree->max_y, octree->max_z);
                }
            }
        }
    }
    return child;
}

