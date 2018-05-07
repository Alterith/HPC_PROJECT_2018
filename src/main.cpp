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
#include "octree.h"
#include "barnes_hut.h"

using namespace std;

/*
 * 
 */

void update_position(node* octree, node* root);
int update_point(node* octree, node* root, int num_ele, double points[][7]);

int main(int argc, char** argv) {

    int i = 0;
    int bound = 100;
    int num_ele = 5;
    int iterations = 10;
    srand(time(NULL)); //just seed the generator

    //mass, pos x,y,z, vel x,y,z
    double point[num_ele][7];
    //create point array
    for (int i = 0; i < num_ele; i++) {
        //mass
        point[i][0] = ((rand() % 20000) / 1000.0) + 1;
        //position
        for(int j = 1; j < 4; j++){
            point[i][j] = ((rand() / (float)RAND_MAX) * 2 * bound) - bound;
        }
        //velocity
        for(int j = 4; j < 7; j++){
            point[i][j] = ((rand() % 40000) - 20000) / 1000.0;
        }
    }
    //insert into tree and run
    for(int i = 0; i < iterations; i++){
        node* test = malloc_node(-1*bound, -1*bound, -1*bound, bound, bound, bound);
        cout << test->min_x << " " << test->min_y << " " << test->min_z << endl;
        cout << test->mid_x << " " << test->mid_y << " " << test->mid_z << endl;
        cout << test->max_x << " " << test->max_y << " " << test->max_z << endl << endl;
        for (int i = 0; i < num_ele; i++) {  
            int ele = insert_node(test, point[i][0], point[i][1], point[i][2], point[i][3], point[i][4], point[i][5], point[i][6]);
        }
        update_position(test, test);
        update_point(test, test, 0, point);
        cout<<"updated points"<<endl;
        free_node(test);
        cout<<"free tree"<<endl;
    }

    
    return 0;
}

void update_position(node* octree, node* root){
    if(octree->num_points == 1){
        //cout<<octree->force<<endl;
        calculate_force(octree, root, 1);
        //cout<<octree->force_x<<endl;
        //cout<<octree->force_y<<endl;
        //cout<<octree->force_z<<endl<<endl;
        // cout<<octree->com_x<<endl;
        // cout<<octree->com_y<<endl;
        // cout<<octree->com_z<<endl<<endl;
    }else{
        for(int i = 0; i < 8; i++){
            if(octree->children[i]){
               update_position(octree->children[i], root);
            }
        }
    }
}
//to start pass num_ele = 0
int update_point(node* octree, node* root, int num_ele, double points[][7]){
    if(octree->num_points == 1){
        //put point in array
        cout<<num_ele<<endl;
        points[num_ele][0] = octree->mass;
        points[num_ele][1] = octree->com_x;
        points[num_ele][2] = octree->com_y;
        points[num_ele][3] = octree->com_z;
        points[num_ele][4] = octree->vel_x;
        points[num_ele][5] = octree->vel_y;
        points[num_ele][6] = octree->vel_z;
        num_ele++;
        return num_ele;
    }else{
        for(int i = 0; i < 8; i++){
            if(octree->children[i]){
               num_ele = update_point(octree->children[i], root, num_ele, points);
            }
        }
    }
    return 0;
}

