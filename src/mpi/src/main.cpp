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

// Settings
const int num_ele = 512;
const int iterations = 1024;
const int bound = 32;
const int writeOut = 1;

void update_position(node *octree, node *root, int world_rank, int body_count);
void update_point(node *octree, array<body, num_ele> *point, int bound, int world_rank, int body_count);

int main(int argc, char **argv)
{
    // Timing
    double t1, t2;
    double tOverhead = 0;
    double tScratch1 = 0;
    double tScratch2 = 0;
    int bodyCount;

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

    // Calculate the number of bodies that each process will handle
    bodyCount = (num_ele / world_size);

    // Start timing runtime
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0)
    {
        t1 = MPI::Wtime();
    }

    // Open file to export point data, only if setting is set
    ofstream positionFile;
    if (writeOut == 1)
    {

        positionFile.open("../visual/positionFile.txt");
    }

    // Seed the random number generator
    srand(time(NULL));

    // Create array that holds points, and find it's size
    unsigned int vecMemorySize = sizeof(array<body, num_ele>) + (sizeof(body) * num_ele);
    array<body, num_ele> *point = new array<body, num_ele>;

    // Point creation, only run on one process
    if (world_rank == 0)
    {

        // Create point array
        for (int i = 0; i < num_ele; i++)
        {
            // Mass
            //double mass = (float)((rand() % 200000000) / 10.0) + 1.0;
            double mass = (float)20000000;

            // Position
            dim3float pos(
                (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound,
                (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound,
                (float)((rand() / (float)RAND_MAX) * 2 * bound) - (float)bound);

            // Velocity
            dim3float vel(
                0.0,  //(float)((rand() % 40000) - 20000) / 1000.0,
                0.0,  //(float)((rand() % 40000) - 20000) / 1000.0,
                0.0); //(float)((rand() % 40000) - 20000) / 1000.0);

            // Insert into point array
            body newBody = create_body(mass, pos, vel);
            (*point)[i] = (newBody);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Create a type for struct dim3float
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

    // Create a type for struct body
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

    // Run simulation for the set number of iterations
    for (int j = 0; j < iterations; j++)
    {
        // Start timing MPI broadcast overhead
        MPI_Barrier(MPI_COMM_WORLD);
        if (world_rank == 0)
        {
            tScratch1 = MPI::Wtime();
        }

        // Broadcast the body array to all processes
        MPI_Bcast((point->data()), num_ele, mpi_body_type, 0, MPI_COMM_WORLD);

        // Stop timing MPI broadcast overhead and add to collective overhead sum
        MPI_Barrier(MPI_COMM_WORLD);
        if (world_rank == 0)
        {
            tOverhead += MPI::Wtime() - tScratch1;
        }

        // Create headnode of octree
        node *headNode = malloc_node(-1 * bound, -1 * bound, -1 * bound, bound, bound, bound);

        // Insert the points in to the tree
        for (int i = 0; i < num_ele; i++)
        {
            int ele = insert_node(headNode, (*point)[i], i);
        }

        // Update positions of the nodes in the tree, then update the points based on the tree
        update_position(headNode, headNode, world_rank, bodyCount);
        update_point(headNode, point, bound, world_rank, bodyCount);

        // Free the tree
        free_node(headNode);

        // A process should only send data once, this is a flag that gets switched ensuring a
        // node does not send more than once
        bool sent = false;

        // Time MPI reduction overhead
        if (world_rank == 0)
        {
            tScratch1 = MPI::Wtime();
        }

        // Repeatedly copy unique points from different processes
        for (int stride = 1; stride < world_size; stride *= 2)
        {
            // Calucalte the amount of points to send at each iteration of the reduction, doubles every time
            int width = (stride) * (num_ele / world_size);

            // Processes that receive the updated points
            if (world_rank % (2 * stride) == 0)
            {
                body *section = &(point->data()[((world_rank + stride) * (width / stride))]);
                //cout << "try:       receive:    " << world_rank << "<-" << world_rank + stride << endl;
                MPI_Recv(section, width, mpi_body_type, world_rank + stride, width, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //cout << "success:   receive:    " << world_rank << "<-" << world_rank + stride << endl;
            }
            // Processes that send the updated points
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

            // Finish timing the MPI reduction and add to cumulative sum
            MPI_Barrier(MPI_COMM_WORLD);
            if (world_rank == 0)
            {
                tOverhead += MPI::Wtime() - tScratch1;
            }
        }

        
        // If writing to file, this will print the list of points to the file
        MPI_Barrier(MPI_COMM_WORLD);
        if ((world_rank == 0) && (writeOut == 1))
        {
            for (int k = 0; k < point->size(); k++)
            {

                positionFile << (*point)[k].com.x << "|" << (*point)[k].com.y << "|" << (*point)[k].com.z << endl;
            }
            positionFile << endl;
        }
    }

    // Close the file when no more updates are going to be written
    if (writeOut == 1)
    {
        positionFile.close();
    }

    // Finish timing the overall time of the simulation
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0)
    {
        t2 = MPI::Wtime();
        cout << "Overhead Time: " << tOverhead << endl;
        cout << "Total Time: " << t2 - t1 << endl;
        
    }

    // Finish with any MPI
    MPI_Finalize();

    return 0;
}

// Update the nodes in the octree
void update_position(node *octree, node *root, int world_rank, int body_count)
{
    if (octree->num_points == 1)
    {
        // Only calulate force if the point lies withing the processes' allocated set of points
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

// Update the points based on the updated tree with force calculations
void update_point(node *octree, array<body, num_ele> *point, int bound, int world_rank, int body_count)
{
    if (octree->num_points == 1)
    {
        // Only update the points that are allocated to a process
        if ((octree->body_num >= world_rank * body_count) && (octree->body_num < (world_rank + 1) * body_count))
        {

            //put point in array
            //octree->vel.cout2();
            octree->com.new_pos(octree->vel, 0.1, bound);
            int num_ele = octree->body_num;
            (*point)[num_ele].mass = octree->mass;
            (*point)[num_ele].com = octree->com;
            (*point)[num_ele].vel = octree->vel;
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (octree->children[i])
            {
                update_point(octree->children[i], point, bound, world_rank, body_count);
            }
        }
    }
}
