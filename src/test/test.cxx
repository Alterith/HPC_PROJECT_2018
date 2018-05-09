#include <vector>

FILE *pipe = popen("gnuplot -persist", "w");

// set axis ranges
fprintf(pipe,"set xrange [0:11]\n");
fprintf(pipe,"set yrange [0:11]\n");

int b = 5;int a;

// to make 10 points
std::vector<int> x (10, 0.0); // x values
std::vector<int> y (10, 0.0); // y values

for (a=0;a<10;a++) // 10 plots
{
    x[a] = a;
    y[a] = // some function of a
    fprintf(pipe,"plot '-'\n");
    // 1 additional data point per plot
    for (int ii = 0; ii <= a; ii++) {
        fprintf(pipe, "%d %d\n", x[ii], y[ii]) // plot `a` points
    }

    fprintf(pipe,"e\n");    // finally, e
    fflush(pipe);   // flush the pipe to update the plot
    usleep(1000000);// wait a second before updating again
}

//  close the pipe
fclose(pipe);
