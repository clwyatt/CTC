// File: canny.h
// Abstract: header for canny.cc
// Created: 08/11/98 by: Chris L. Wyatt
//
// Note: See origional code author note in canny.cc
//


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define VERBOSE 1
#define BOOSTBLURFACTOR 90.0
#define NOEDGE 255
#define POSSIBLE_EDGE 128
#define EDGE 0


void canny(unsigned short *image, int rows, int cols, float sigma,
         float tlow, float thigh, unsigned char **edge, char *fname);

void gaussian_smooth(unsigned short *image, int rows, int cols, float sigma,
        short int **smoothedim);

void make_gaussian_kernel(float sigma, float **kernel, int *windowsize);

void derrivative_x_y(short int *smoothedim, int rows, int cols,
        short int **delta_x, short int **delta_y);

void magnitude_x_y(short int *delta_x, short int *delta_y, int rows, int cols,
        short int **magnitude);

void apply_hysteresis(short int *mag, unsigned char *nms, int rows, int cols,
        float tlow, float thigh, unsigned char *edge);

void radian_direction(short int *delta_x, short int *delta_y, int rows,
    int cols, float **dir_radians, int xdirtag, int ydirtag);

double angle_radians(double x, double y);

void follow_edges(unsigned char *edgemapptr, short *edgemagptr, short lowval,
   int cols);

void non_max_supp(short *mag, short *gradx, short *grady, int nrows, int ncols,
    unsigned char *result);
