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
#include <fstream>
using namespace std;

const int writeOut = 1;

/*
 * 
 */

void update_position(node *octree, node *root);
void update_point(node *octree, vector<body *> *point, int bound);

int main(int argc, char **argv)
{
	// Open file to export point data, only if setting is set
    ofstream positionFile;
	if (writeOut == 1)
    {
        positionFile.open("../visual/positionFile.txt");
    }
	clock_t start_total = clock(), end_total;
    int i = 0;
    int bound = 32;
    const int num_ele = 512;
    int iterations = 1024;
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
    for (int j = 0; j < iterations; j++)
    {
        node *test = malloc_node(-1 * bound, -1 * bound, -1 * bound, bound, bound, bound);
        for (int i = 0; i < num_ele; i++)
        {
            int ele = insert_node(test, (*point)[i], i);
        }
		//cout<<j<<": calculating force"<<endl;
        update_position(test, test);
		//cout<<j<<": updating point"<<endl;
        update_point(test, point, bound);
        //cout<<"updated points"<<endl;
        free_node(test);
        //cout<<"free tree"<<endl;
		if (writeOut == 1)
        {
            for (int idx = 0; idx < num_ele; idx++)
            {
                positionFile << (*point)[idx]->com.x << "|" << (*point)[idx]->com.y << "|" << (*point)[idx]->com.x << endl;
            }
            positionFile << endl;
        }
    }

    for (i = 0; i < num_ele; i++)
    {
        free((*point)[i]);
    }
	
	end_total = clock();
	printf("Overhead: %f\n",0.0);
	float total_time = end_total - start_total;
	total_time /= (float)CLOCKS_PER_SEC;
	printf("Total: %f\n",total_time);
	
	// Close the file when no more updates are going to be written
    if (writeOut == 1)
    {
        positionFile.close();
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
void update_point(node *octree, vector<body *> *point, int bound)
{
    if (octree->num_points == 1)
    {
        //put point in array
        //octree->com.cout2();
        octree->com.new_pos(octree->vel, 0.01, bound);
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
                update_point(octree->children[i], point, bound);
            }
        }
    }
}
