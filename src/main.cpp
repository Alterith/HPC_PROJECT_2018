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

using namespace std;

/*
 * 
 */

void update_position(node* octree, node* root);
void update_point(node* octree, node* root, vector<vector<float> > *point);

int main(int argc, char** argv) {

    int i = 0;
    int bound = 512;
    const int num_ele = 2000;
    int iterations = 100;
    srand(time(NULL)); //just seed the generator

    //mass, pos x,y,z, vel x,y,z
    //double point[num_ele][7];
    vector<vector<float>> *point = new vector<vector<float>>(num_ele);
    //create point array
    for (int i = 0; i < num_ele; i++) {
        //mass
        (*point)[i].push_back((float)((rand() % 20000) / 1000.0) + 1.0);
        //position
        for(int j = 1; j < 4; j++){
            (*point)[i].push_back((float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound);
        }
        //velocity
        for(int j = 4; j < 7; j++){
            (*point)[i].push_back((float)((rand() % 40000) - 20000) / 1000.0);
        }
    }
    //insert into tree and run
    for(int i = 0; i < iterations; i++){
        node* test = malloc_node(-1*bound, -1*bound, -1*bound, bound, bound, bound);
        //cout << test->min.x << " " << test->min.y << " " << test->min.z << endl;
        //cout << test->mid.x << " " << test->mid.y << " " << test->mid.z << endl;
        //cout << test->max.x << " " << test->max.y << " " << test->max.z << endl << endl;
        for (int i = 0; i < num_ele; i++) {  
            int ele = insert_node(test, (*point)[i][0], (*point)[i][1], (*point)[i][2], (*point)[i][3], (*point)[i][4], (*point)[i][5], (*point)[i][6], i);
        }
        update_position(test, test);
        update_point(test, test, point);
        //cout<<"updated points"<<endl;
        free_node(test);
        //cout<<"free tree"<<endl;
    }

    
    return 0;
}

void update_position(node* octree, node* root){
    if(octree->num_points == 1){
        calculate_force(octree, root, 1);
    }else{
        for(int i = 0; i < 8; i++){
            if(octree->children[i]){
               update_position(octree->children[i], root);
            }
        }
    }
}

//to start pass num_ele = 0
void update_point(node* octree, node* root, vector<vector<float> > *point){
    if(octree->num_points == 1){
        //put point in array
		//octree->com.cout2();
		octree->com.new_pos(octree->vel, 0.1);
		int num_ele = octree->body_num;
        (*point)[num_ele][0] = octree->mass;
        (*point)[num_ele][1] = octree->com.x;
        (*point)[num_ele][2] = octree->com.y;
        (*point)[num_ele][3] = octree->com.z;
        (*point)[num_ele][4] = octree->vel.x;
        (*point)[num_ele][5] = octree->vel.y;
        (*point)[num_ele][6] = octree->vel.z;
    }else{
        for(int i = 0; i < 8; i++){
            if(octree->children[i]){
               update_point(octree->children[i], root, point);
            }
        }
    }
}

