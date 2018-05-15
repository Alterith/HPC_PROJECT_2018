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
#include "arr_node.h"

using namespace std;


/*
 * function definitions
 */

void update_point(arr_node* arr_tree, int* h_body_pos, vector<body *> *point, int num_ele);
int get_points(int n, node* octree_node);
int convert_tree_to_array(int n, node* octree_node, arr_node* arr_tree, int* h_body_pos, int* pos);

/*
 *
 */


/*
 * kernel definitions
 */

__global__ void test_kernel(int n, int num_ele, arr_node* arr_tree, int* pos){
    //obtain index
    int idx = threadIdx.x + blockIdx.x*blockDim.x;
    arr_node* singular_body;
    if(idx<num_ele){
        singular_body = &arr_tree[pos[idx]];
        printf("%d\t%d\t%d\n", pos[idx], singular_body->furthest_right, singular_body->body_num);
        //TODO: do traversal and force update calculation
        __syncthreads();
        //TODO: do point update
    }
}
 
 /*
  *
  */

int main(int argc, char **argv)
{

    int i = 0;
    int bound = 512;
    const int num_ele = 50;
    int iterations = 2;
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
        for (int i = 0; i < num_ele; i++)
        {
            int ele = insert_node(test, (*point)[i], i);
        }

        //just some testing for array representation;
        int n = get_points(0, test);
        cout<<n<<endl;
        //declare pointers to host and device memory
        arr_node* h_arr_tree;
        arr_node* d_arr_tree;
        //individual body positions in array
        int* h_body_pos;
        int* d_body_pos;
        //allocate host memory
        h_arr_tree = (arr_node*)malloc(n*sizeof(arr_node));
        h_body_pos = (int*)malloc(num_ele*sizeof(int));
        //populate host array
        int pos = 0;
        int garbage = convert_tree_to_array(0, test, h_arr_tree, h_body_pos, &pos);
        //free the tree
        free_node(test);
        
        for(int k = 0; k < num_ele; k++){
            printf("%d\t%d\t%d\n", h_body_pos[k], h_arr_tree[h_body_pos[k]].furthest_right, h_arr_tree[h_body_pos[k]].body_num);
        }
        cout<<endl;

        //device memory allocation
        cudaMalloc((void **)&d_arr_tree, n*sizeof(arr_node));
        cudaMalloc((void **)&d_body_pos, num_ele*sizeof(int));
        
        int nblocks  = 3;
        int nthreads = num_ele/nblocks;
        //check if enough threads exist
        if(nthreads*nblocks != num_ele){
            nthreads++;
        }

        //copy relevant data to device
        cudaMemcpy(d_arr_tree, h_arr_tree, n*sizeof(arr_node), cudaMemcpyHostToDevice);
        cudaMemcpy(d_body_pos, h_body_pos, num_ele*sizeof(int), cudaMemcpyHostToDevice);
        
        test_kernel<<<nblocks,nthreads>>>(n, num_ele, d_arr_tree, d_body_pos);
        cudaDeviceSynchronize();
        cudaMemcpy(h_arr_tree, d_arr_tree, n*sizeof(arr_node), cudaMemcpyDeviceToHost);
        //end of array representation testing

        //insert function updating the bodies
        update_point(h_arr_tree, h_body_pos, point, num_ele);

        //free memory
        free(h_arr_tree);
        free(h_body_pos);
        cudaFree(d_arr_tree);
        cudaFree(d_body_pos);
    }
    return 0;
}

//to start pass num_ele = 0
void update_point(arr_node* arr_tree, int* h_body_pos, vector<body *> *point, int num_ele)
{
    for(int i = 0; i < num_ele; i++){
        //get singular body
        arr_node temp = arr_tree[h_body_pos[i]];
        //get vector index
        int idx = temp.body_num;
        //allocate updated values
        (*point)[idx]->mass = temp.mass;
        (*point)[idx]->com.x = temp.com_x;
        (*point)[idx]->com.y = temp.com_y;
        (*point)[idx]->com.z = temp.com_z;
        (*point)[idx]->vel.x = temp.vel_x;
        (*point)[idx]->vel.y = temp.vel_y;
        (*point)[idx]->vel.z = temp.vel_z;
    }
}

// get num points in tree

int get_points(int n, node* octree_node){

    n +=1;
    for (int i = 0; i < 8; i++)
    {
        if (octree_node->children[i])
        {
            n = get_points(n, octree_node->children[i]);
        }
    }
    return n;

}

int convert_tree_to_array(int n, node* octree_node, arr_node* arr_tree, int* h_body_pos, int* pos){
    //store relevant data into array from node
    arr_tree[n].body_num = octree_node->body_num;
    arr_tree[n].com_x = octree_node->com.x;
    arr_tree[n].com_y = octree_node->com.y;
    arr_tree[n].com_z = octree_node->com.z;
    arr_tree[n].mass = octree_node->mass;
    arr_tree[n].vel_x = octree_node->vel.x;
    arr_tree[n].vel_y = octree_node->vel.y;
    arr_tree[n].vel_z = octree_node->vel.z;
    if(arr_tree[n].body_num != -1){
        h_body_pos[*pos] = n;
        *pos = *pos + 1;
    }
    int n1 = n;
    n +=1;
    //traverse children recursively
    for (int i = 0; i < 8; i++)
    {
        if (octree_node->children[i])
        {
            n = convert_tree_to_array(n, octree_node->children[i], arr_tree, h_body_pos, pos);
        }
    }
    //store furthest right node for skipping
    arr_tree[n1].furthest_right = n-1;
    return n;
}