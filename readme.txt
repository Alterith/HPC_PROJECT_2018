Each of the implementations are located in src.
Visual is what we use to create the video.

To execute a specific method open the folder in terminal and type bash run.sh.

CUDA is slightly different, you can execute the code by typing ./project.out or specify the bound, number of iterations and number of points such as ./project.out <bound> <number of points> <number of iterations>.

Or you may change these variables in each projects main.cpp/.cu, they will be located at the top.
the variable names are with an example of numbers we ran:

	int bound;;//32
    int num_ele;//4096
    int iterations;//128
