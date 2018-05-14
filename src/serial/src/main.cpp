/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: alterith
 *
 * Created on 05 May 2018, 3:40 PM
 */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <vector>
#include "octree.h"
#include "barnes_hut.h"
#include "body.h"

using namespace std;

/*
 * 
 */

void update_position(node *octree, node *root);
void update_point(node *octree, vector<body *> *point);

int main(int argc, char **argv)
{

    int i = 0;
    int bound = 512;
    const int num_ele = 5;
    int iterations = 100;
    srand(time(NULL)); //just seed the generator

    //mass, pos x,y,z, vel x,y,z
    //double point[num_ele][7];
    vector<body *> *point = new vector<body *>(num_ele);
    //create point array
    for (i = 0; i < num_ele; i++)
    {
        //mass
        double mass = (float)((rand() % 20000) / 1000.0) + 1.0;

        //position
        dim3float pos(
            (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound,
            (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound,
            (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound);

        //velocity
        dim3float vel(
            (float)((rand() % 40000) - 20000) / 1000.0,
            (float)((rand() % 40000) - 20000) / 1000.0,
            (float)((rand() % 40000) - 20000) / 1000.0);

        //insert
        body *newBody = create_body(mass, pos, vel);
        (*point)[i] = (newBody);
    }
    //insert into tree and run
    for (int i = 0; i < iterations; i++)
    {
        node *test = malloc_node(-1 * bound, -1 * bound, -1 * bound, bound, bound, bound);
        //cout << test->min.x << " " << test->min.y << " " << test->min.z << endl;
        //cout << test->mid.x << " " << test->mid.y << " " << test->mid.z << endl;
        //cout << test->max.x << " " << test->max.y << " " << test->max.z << endl << endl;
        for (int i = 0; i < num_ele; i++)
        {
            int ele = insert_node(test, (*point)[i], i);
        }
        update_position(test, test);
        update_point(test, point);
        //cout<<"updated points"<<endl;
        free_node(test);
        //cout<<"free tree"<<endl;
    }

    return 0;
}

void update_position(node *octree, node *root)
{
    if (octree->num_points == 1)
    {
        calculate_force(octree, root, 1);
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (octree->children[i])
            {
                update_position(octree->children[i], root);
            }
        }
    }
}

//to start pass num_ele = 0
void update_point(node *octree, vector<body *> *point)
{
    if (octree->num_points == 1)
    {
        //put point in array
        octree->com.cout2();
        octree->com.new_pos(octree->vel, 0.1);
        int num_ele = octree->body_num;
        (*point)[num_ele]->mass = octree->mass;
        (*point)[num_ele]->com = octree->com;
        (*point)[num_ele]->vel = octree->vel;
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (octree->children[i])
            {
                update_point(octree->children[i], point);
            }
        }
    }
}
