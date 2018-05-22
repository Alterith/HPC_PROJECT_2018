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
#include "body.h"
#include "arr_node.h"
// Includes CUDA
#include <cuda_runtime.h>

// Utilities and timing functions
#include <helper_functions.h>    // includes cuda.h and cuda_runtime_api.h

// CUDA helper functions
#include <helper_cuda.h>         // helper functions for CUDA error check

#define THETA 0.5
#define G -6.674*(pow(10,-11))
#define delta_t 0.1

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess)
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

using namespace std;

const int writeOut = 1;


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
 * device definitions
 */
__device__ double distance_com(arr_node a, arr_node b) {
    double distance = (double)sqrt((double)pow((a.com_x - b.com_x), 2) + (double)pow((a.com_y - b.com_y), 2) + (double)pow((a.com_z - b.com_z), 2));
    return distance;
}

__device__ double size_region(arr_node a) {
    //perimeter
    double size = pow((a.max_x - a.min_x), 2);
    return size;
}

__global__ void body_update_kernel(int n, int num_ele, arr_node* arr_tree, int* pos, int bound){
    //obtain index
    int idx = threadIdx.x + blockIdx.x*blockDim.x;
    arr_node* singular_body;
    if(idx<num_ele){
        //use direct access to change things on source
        singular_body = &arr_tree[pos[idx]];
        //printf("Device: %d\t%d\t%d\n", pos[idx], singular_body->furthest_right, singular_body->body_num);
        //TODO: do traversal and force update calculation
        for(int i = 0; i<n; i++){
            //skip body if its the same point, pos is a singular body index in the arr_tree array
            if(i != pos[idx]){
                //calculate distance
                double dist = distance_com(*singular_body, arr_tree[i]);
                //if singular body no need to check if well seperated
                if(arr_tree[i].body_num != -1){
                    // newtons law of universal gravitation
                    double force_x = (double) ((G * singular_body->mass * arr_tree[i].mass) / ((double)pow(dist, 3)+ 0.3))*(singular_body->com_x - arr_tree[i].com_x);
                    double force_y = (double) ((G * singular_body->mass * arr_tree[i].mass) / ((double)pow(dist, 3)+ 0.3))*(singular_body->com_y - arr_tree[i].com_y);
                    double force_z = (double) ((G * singular_body->mass * arr_tree[i].mass) / ((double)pow(dist, 3)+ 0.3))*(singular_body->com_z - arr_tree[i].com_z);
					                  
					//printf("%.11lf\n", force_x);
                    //10.10
                    if((force_x!=force_x)){
                      force_x = 1000;
                      //printf("force_x\n");
                    }
                    if((force_y!=force_y)){
                      force_y = 1000;
                      //printf("force_y\n");
                    }
                    if((force_z!=force_z)){
                      force_z = 1000;
                      //printf("force_z\n");
                    }

					
                    singular_body->force_x += force_x;
                    singular_body->force_y += force_y;
                    singular_body->force_z += force_z;
                    //printf("Distance: %lf\n", dist);
                }else{

                    //obtain perimeter of octant
                    double perimeter = size_region(arr_tree[i]);
                    //check if well separated
                    //printf("%lf\t%lf\t%lf\n", perimeter/dist, perimeter, dist);
                    if ((perimeter / dist) <= THETA) {
                        // we may treat the nodes in this octant as one and proceed
                        //newtons law of universal gravitation: (G*m1*m2/r^3)*r_(x,y,z)
                        double force_x = (double) ((G * singular_body->mass * arr_tree[i].mass) / ((double)pow(dist, 3)+ 0.3))*(singular_body->com_x - arr_tree[i].com_x);
                        double force_y = (double) ((G * singular_body->mass * arr_tree[i].mass) / ((double)pow(dist, 3)+ 0.3))*(singular_body->com_y - arr_tree[i].com_y);
                        double force_z = (double) ((G * singular_body->mass * arr_tree[i].mass) / ((double)pow(dist, 3)+ 0.3))*(singular_body->com_z - arr_tree[i].com_z);

						if((force_x!=force_x)){
		                  force_x = 1000;
		                  //printf("force_x\n");
		                }
		                if((force_y!=force_y)){
		                  force_y = 1000;
		                  //printf("force_y\n");
		                }
		                if((force_z!=force_z)){
		                  force_z = 1000;
		                  //printf("force_z\n");
		                }

						singular_body->force_x += force_x;
                        singular_body->force_y += force_y;
                        singular_body->force_z += force_z;

                        //skip children
                        //printf("skipped\n");
                        i = arr_tree[i].furthest_right;
                    }
                }
            }
        }
        __syncthreads();
        //TODO: do point update
        //do velocity calc change here
        singular_body->vel_x = (double)(singular_body->vel_x + (singular_body->force_x/singular_body->mass)*delta_t); //(double)fmod((double)(singular_body->vel_x + (singular_body->force_x/singular_body->mass)*delta_t), (double)100.0);
        singular_body->vel_y = (double)(singular_body->vel_y + (singular_body->force_y/singular_body->mass)*delta_t);//(double)fmod((double)(singular_body->vel_y + (singular_body->force_y/singular_body->mass)*delta_t), (double)100.0);
        singular_body->vel_z = (double)(singular_body->vel_z + (singular_body->force_z/singular_body->mass)*delta_t);//(double)fmod((double)(singular_body->vel_z + (singular_body->force_z/singular_body->mass)*delta_t), (double)100.0);
        //update position
		singular_body->com_x = singular_body->com_x + singular_body->vel_x*delta_t;
        singular_body->com_y = singular_body->com_y + singular_body->vel_y*delta_t;
        singular_body->com_z = singular_body->com_z + singular_body->vel_z*delta_t;


		while(abs(singular_body->com_x)>bound){
			double rem = 0;
			if(singular_body->com_x > bound){
				rem = singular_body->com_x - bound;
				singular_body->com_x = (-1)*bound + (double)fmod(rem, (double)(bound-1));
			}else if(singular_body->com_x < (-1)*bound){
				rem = abs(singular_body->com_x)-abs(bound);
				singular_body->com_x = bound - (float)fmod(rem, (double)bound);
			}
			//printf("%lf\n", rem);
			//std::cout<<rem<<std::endl;
		}

		while(abs(singular_body->com_y)>bound){
			double rem = 0;
			if(singular_body->com_y > bound){
				rem = singular_body->com_y - bound;
				singular_body->com_y = (-1)*bound + (double)fmod(rem, (double)(bound-1));
			}else if(singular_body->com_y < (-1)*bound){
				rem = abs(singular_body->com_y)-abs(bound);
				singular_body->com_y = bound - (double)fmod(rem, (double)(bound-1));
			}
			//printf("%lf\n", rem);
			//std::cout<<rem<<std::endl;
		}

		while(abs(singular_body->com_z)>bound){
			double rem = 0;
			if(singular_body->com_z > bound){
				rem = singular_body->com_z - bound;
				singular_body->com_z = (-1)*bound + (double)fmod(rem, (double)(bound-1));
			}else if(singular_body->com_z < (-1)*bound){
				rem = abs(singular_body->com_z)-abs(bound);
				singular_body->com_z = bound - (double)fmod(rem, (double)(bound-1));
			}
			//printf("%lf\n", rem);
			//std::cout<<rem<<std::endl;
		}
    }
}

 /*
  *
  */

int main(int argc, char **argv)
{
	// Open file to export point data, only if setting is set
    ofstream positionFile;
    if (writeOut == 1)
    {
        positionFile.open("../visual/positionFile.txt");
    }
	clock_t start_total = clock(), end_total;
	//first cuda call takes longer so this is to allow for fair comparison by removing that overhead
    float *dData = NULL;
    gpuErrchk(cudaMalloc((void **) &dData, 1));
    gpuErrchk(cudaFree(dData));

    int i = 0;
    int bound;;//32
    int num_ele;//4096
    int iterations;//128
	if(argc == 4){
		sscanf (argv[1],"%d",&bound);
		sscanf (argv[2],"%d",&num_ele);
		sscanf (argv[3],"%d",&iterations);
	}else{
		bound = 32;
		num_ele = 512;
		iterations = 128;
	}
	
    srand(time(NULL)); //just seed the generator

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
	// kernel time over all iterations
	float kernel_time = 0;
	float overhead_time = 0;
    //insert into tree and run
    for (int j = 0; j < iterations; j++)
    {	
		
        node *test = malloc_node(-1 * bound, -1 * bound, -1 * bound, bound, bound, bound);
        for (int i = 0; i < num_ele; i++)
        {
            int ele = insert_node(test, (*point)[i], i);
        }
		//overhead timing
		clock_t start, end;
        start = clock();
        //array representation
        int n = get_points(0, test);
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
		//overhead
		end = clock();
		overhead_time += end - start;        
		//free the tree
        free_node(test);


		// overhead
		start = clock();
        //device memory allocation
        cudaMalloc((void **)&d_arr_tree, n*sizeof(arr_node));
        cudaMalloc((void **)&d_body_pos, num_ele*sizeof(int));

        int nblocks  = num_ele/1024;
		if(!nblocks){
			nblocks++;
		}
        int nthreads = num_ele/nblocks;
        //check if enough threads exist
        if(nthreads*nblocks != num_ele){
            nthreads++;
        }

        //copy relevant data to device
        cudaMemcpy(d_arr_tree, h_arr_tree, n*sizeof(arr_node), cudaMemcpyHostToDevice);
        cudaMemcpy(d_body_pos, h_body_pos, num_ele*sizeof(int), cudaMemcpyHostToDevice);
		// overhead
		end = clock();
		overhead_time += end - start;
		
		float time;
		cudaEvent_t start_k, stop_k;

		gpuErrchk( cudaEventCreate(&start_k) );
		gpuErrchk( cudaEventCreate(&stop_k) );
		gpuErrchk( cudaEventRecord(start_k, 0) );
        body_update_kernel<<<nblocks,nthreads>>>(n, num_ele, d_arr_tree, d_body_pos, bound);
        //gpuErrchk( cudaPeekAtLastError() );
		gpuErrchk( cudaDeviceSynchronize() );
		gpuErrchk( cudaEventRecord(stop_k, 0) );
		gpuErrchk( cudaEventSynchronize(stop_k) );
		gpuErrchk( cudaEventElapsedTime(&time, start_k, stop_k) );
		//printf("Kernel: %.11f ms\n",sdkGetTimerValue(&timer));
		kernel_time += time;

		// overhead
		start = clock();
        //end of array representation testing
        cudaMemcpy(h_arr_tree, d_arr_tree, n*sizeof(arr_node), cudaMemcpyDeviceToHost);
        //end of array representation testing

        //insert function updating the bodies
        update_point(h_arr_tree, h_body_pos, point, num_ele);
		// overhead
		end = clock();
		overhead_time += end - start;
        //free memory
        free(h_arr_tree);
        free(h_body_pos);
        cudaFree(d_arr_tree);
        cudaFree(d_body_pos);

		if (writeOut == 1)
        {
            for (int idx = 0; idx < num_ele; idx++)
            {
                positionFile << (*point)[idx]->com.x << "|" << (*point)[idx]->com.y << "|" << (*point)[idx]->com.x << endl;
            }
            positionFile << endl;
        }
		//cout<<j<<endl;
    }

	for (i = 0; i < num_ele; i++)
    {
        free((*point)[i]);
    }	
	
	end_total = clock();
	kernel_time /= (float)1000;
	printf("Kernel: \t%f\n",kernel_time);
	overhead_time /= (float)CLOCKS_PER_SEC;
	printf("Overhead: %f\n",overhead_time);
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

//to start pass num_ele = 0
void update_point(arr_node* arr_tree, int* h_body_pos, vector<body *> *point, int num_ele)
{
    for(int i = 0; i < num_ele; i++){
        //get singular body
        arr_node temp = arr_tree[h_body_pos[i]];
        //get vector index
        int idx = temp.body_num;
        //allocate updated values
		//(*point)[idx]->com.cout2();
        (*point)[idx]->mass = temp.mass;
        (*point)[idx]->com.x = temp.com_x;
        (*point)[idx]->com.y = temp.com_y;
        (*point)[idx]->com.z = temp.com_z;
        (*point)[idx]->vel.x = temp.vel_x;
        (*point)[idx]->vel.y = temp.vel_y;
        (*point)[idx]->vel.z = temp.vel_z;
		//if(i == 3028)
			//(*point)[idx]->com.cout2();
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
    arr_tree[n].max_x = octree_node->max.x;
    arr_tree[n].max_y = octree_node->max.y;
    arr_tree[n].max_z = octree_node->max.z;
    arr_tree[n].min_x = octree_node->min.x;
    arr_tree[n].min_y = octree_node->min.y;
    arr_tree[n].min_z = octree_node->min.z;
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
