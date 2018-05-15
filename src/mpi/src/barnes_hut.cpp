/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "barnes_hut.h"
#include <cmath>
#include <iostream>

#define THETA 0.5
#define G 6.674*(pow(10,-11))
#define delta_t 0.1

//obtain the distance between 2 nodes com a will represent the current node and b will represent an arbitrary node in the tree

double distance_com(node* a, node* b) {
    double distance = (double)sqrt((double)pow((a->com.x - b->com.x), 2) + (double)pow((a->com.y - b->com.y), 2) + (double)pow((a->com.z - b->com.z), 2));
    return distance;
}

//obtain size of node(s), it will be considered the area of a face, maybe parameter can change later

double size_region(node* a) {
    double size = 6*pow((a->max.x - a->min.x), 2);
    return size;
}

void calculate_force(node* a, node* b, int phase) {
    //starting at the root we calculate all forces, also we exclude a calculation if the node has the same address as 'a'
    if (a != b) {
        //compute distance between nodes
        double d = distance_com(a, b);
        //only 1 element thus no need to check if well separated
        if (b->num_points == 1) {
            double force_x = (double) ((G * a->mass * b->mass) / (double)pow(d, 3))*(a->com.x - b->com.x);
            double force_y = (double) ((G * a->mass * b->mass) / (double)pow(d, 3))*(a->com.y - b->com.y);
            double force_z = (double) ((G * a->mass * b->mass) / (double)pow(d, 3))*(a->com.z - b->com.z);
            a->force.x += force_x;
            a->force.y += force_y;
            a->force.z += force_z;
        } else {
            //check distance and size compare to theta
            double s = size_region(b);

            //if ratio is greater than theta then it is not well separated
            if ((s / d) <= THETA) {
                // we may treat the nodes in this octant as one and proceed
                //newtons law of universal gravitation: (G*m1*m2/r^3)*r_(x,y,z)
            	double force_x = (double) ((G * a->mass * b->mass) / (double)pow(d, 3))*(a->com.x - b->com.x);
            	double force_y = (double) ((G * a->mass * b->mass) / (double)pow(d, 3))*(a->com.y - b->com.y);
            	double force_z = (double) ((G * a->mass * b->mass) / (double)pow(d, 3))*(a->com.z - b->com.z);
            	a->force.x += force_x;
            	a->force.y += force_y;
            	a->force.z += force_z;
                //std::cout<<"Separated"<<std::endl;
            } else {
                //not well separated thus traverse children
                for (int i = 0; i < 8; i++) {
                    //if the child has a point associated with it
                    if(b->children[i]){
                        //recursively call force calculation
                        calculate_force(a, b->children[i], 0);
                    }
                }
            }
        }
    }
    
    if(phase){
        //do velocity calc change here
        a->vel.x = a->vel.x + (a->force.x/a->mass)*delta_t;
        a->vel.y = a->vel.y + (a->force.y/a->mass)*delta_t;
        a->vel.z = a->vel.z + (a->force.z/a->mass)*delta_t;
    }
}
