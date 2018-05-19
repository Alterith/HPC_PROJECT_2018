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
#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "octree.h"
#include "barnes_hut.h"
#include "body.h"

using namespace std;

/*
 * 
 */

const int num_ele = 256;

void update_position(node *octree, node *root, int world_rank, int body_count);
void update_point(node *octree, array<body, num_ele> *point, int bound);

double t1, t2;
double tOverhead = 0;
double tScratch1 = 0;
double tScratch2 = 0;

int main(int argc, char **argv)
{
    int bound = 32;

    int iterations = 32;

    /*
    MPI INITIALISATION
    */
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    /*
    MPI INITIALISATION COMPLETE
    */

    if (world_rank == 0)
    {
        t1 = MPI_Wtime();
    }

    ofstream positionFile;
    positionFile.open("../Visual/positionFile.txt");

    srand(time(NULL)); //just seed the generator
    unsigned int vecMemorySize = sizeof(array<body, num_ele>) + (sizeof(body) * num_ele);
    array<body, num_ele> *point = new array<body, num_ele>;

    //cout << sizeof(*point) << " ";
    //cout << vecMemorySize << endl;

    if (world_rank == 0)
    {

        //create point array
        for (int i = 0; i < num_ele; i++)
        {
            //mass
            double mass = (float)((rand() % 200000000) / 10.0) + 1.0;

            //position
            dim3float pos(
                (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound,
                (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound,
                (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound);

            //velocity
            dim3float vel(
                0.0,  //(float)((rand() % 40000) - 20000) / 1000.0,
                0.0,  //(float)((rand() % 40000) - 20000) / 1000.0,
                0.0); //(float)((rand() % 40000) - 20000) / 1000.0);

            //insert
            body newBody = create_body(mass, pos, vel);
            (*point)[i] = (newBody);
        }
    }

    int bodyCount = (num_ele / world_size);

    /* create a type for struct dim3float */
    const int nitemsDim3float = 3;
    int blocklengthsD[3] = {1, 1, 1};
    MPI_Datatype typesD[3] = {MPI_FLOAT, MPI_INT, MPI_FLOAT};
    MPI_Datatype mpi_dim3float_type;
    MPI_Aint offsetsD[3];

    offsetsD[0] = offsetof(dim3float, x);
    offsetsD[1] = offsetof(dim3float, y);
    offsetsD[2] = offsetof(dim3float, z);

    MPI_Type_create_struct(nitemsDim3float, blocklengthsD, offsetsD, typesD, &mpi_dim3float_type);
    MPI_Type_commit(&mpi_dim3float_type);

    /* create a type for struct body */
    const int nitemsBody = 3;
    int blocklengthsB[3] = {1, 1, 1};
    MPI_Datatype typesB[3] = {MPI_DOUBLE, mpi_dim3float_type, mpi_dim3float_type};
    MPI_Datatype mpi_body_type;
    MPI_Aint offsetsB[3];

    offsetsB[0] = offsetof(body, mass);
    offsetsB[1] = offsetof(body, com);
    offsetsB[2] = offsetof(body, vel);

    MPI_Type_create_struct(nitemsBody, blocklengthsB, offsetsB, typesB, &mpi_body_type);
    MPI_Type_commit(&mpi_body_type);

    //insert into tree and run
    for (int j = 0; j < iterations; j++)
    {
        if (world_rank == 0)
        {
            tScratch1 = MPI_Wtime();
        }
        MPI_Bcast((point->data()), num_ele, mpi_body_type, 0, MPI_COMM_WORLD);
        if (world_rank == 0)
        {
            tOverhead += MPI_Wtime() - tScratch1;
        }
        node *test = malloc_node(-1 * bound, -1 * bound, -1 * bound, bound, bound, bound);

        for (int i = 0; i < num_ele; i++)
        {
            int ele = insert_node(test, (*point)[i], i);
        }
        update_position(test, test, world_rank, bodyCount);
        update_point(test, point, bound);
        //cout<<"updated points"<<endl;
        free_node(test);
        //cout<<"free tree"<<endl;

        bool sent = false;

        if (world_rank == 0)
        {
            tScratch1 = MPI_Wtime();
        }
        for (int stride = 1; stride < world_size; stride *= 2)
        {
            int width = (stride) * (num_ele / world_size);

            if (world_rank % (2 * stride) == 0)
            {
                body *section = &(point->data()[((world_rank + stride) * (width / stride))]);
                //cout << "try:       receive:    " << world_rank << "<-" << world_rank + stride << endl;
                MPI_Recv(section, width, mpi_body_type, world_rank + stride, width, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //cout << "success:   receive:    " << world_rank << "<-" << world_rank + stride << endl;
            }
            else
            {
                if (!sent)
                {
                    sent = true;
                    body *section = &(point->data()[world_rank * (width / stride)]);
                    //cout << "try:       send:       " << world_rank << "->" << world_rank - stride << endl;
                    MPI_Send(section, width, mpi_body_type, world_rank - stride, width, MPI_COMM_WORLD);
                    //cout << "success:   send:       " << world_rank << "->" << world_rank - stride << endl;
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);
            if (world_rank == 0)
            {
                tOverhead += MPI_Wtime() - tScratch1;
            }
        }
        if (world_rank == 0)
        {
            for (int k = 0; k < point->size(); k++)
            {

                //positionFile << (*point)[k].com.x << "|" << (*point)[k].com.y << "|" << (*point)[k].com.z << endl;
            }
            //positionFile << endl;
        }
    }
    positionFile.close();

    if (world_rank == 0)
    {
        t2 = MPI_Wtime();
        cout << "Total Time: " << t2 - t1 << endl;
        cout << "Overhead Time: " << tOverhead << endl;
    }

    MPI_Finalize();

    return 0;
}

void update_position(node *octree, node *root, int world_rank, int body_count)
{
    if (octree->num_points == 1)
    {
        if ((octree->body_num >= world_rank * body_count) && (octree->body_num < (world_rank + 1) * body_count))
        {
            calculate_force(octree, root, 1);
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (octree->children[i])
            {
                update_position(octree->children[i], root, world_rank, body_count);
            }
        }
    }
}

//to start pass num_ele = 0
void update_point(node *octree, array<body, num_ele> *point, int bound)
{
    if (octree->num_points == 1)
    {
        //put point in array
        //octree->vel.cout2();
        octree->com.new_pos(octree->vel, 0.1, bound);
        int num_ele = octree->body_num;
        (*point)[num_ele].mass = octree->mass;
        (*point)[num_ele].com = octree->com;
        (*point)[num_ele].vel = octree->vel;
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
