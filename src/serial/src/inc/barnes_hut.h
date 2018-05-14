/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   barnes_hut.h
 * Author: alterith
 *
 * Created on 06 May 2018, 8:37 PM
 */

#ifndef BARNES_HUT_H
#define BARNES_HUT_H

#include "octree.h"
//obtain the distance between 2 nodes com a will represent the current node and b will represent an arbitrary node in the tree
double distance_com(node* a, node* b);

//obtain size of node(s), it will be considered the area of a face, maybe parameter can change later
double size_region(node* a);

//phase 0 is a recursive call, phase 1 is the initial call and the force is updated here
void calculate_force(node* a, node* b, int phase);



#endif /* BARNES_HUT_H */

