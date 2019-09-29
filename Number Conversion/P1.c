// P1 Assignment
// Author: Tyler Lucero
// Date:   1/24/2019
// Class:  CS270
// Email:  t1bl3r@cs.colostate.edu

// Include files
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void computeCircle(double radius, double *addressOfArea)
{
    // Compute area
    double result = (3.141593 * radius * radius);

    // return result to pointer
    *addressOfArea = result;
}

void computeTriangle(double side, double *addressOfArea)
{
    // Compute area
    double result = (0.433013 * side * side);

    // return result to pointer
    *addressOfArea = result;
}

void computeSquare(double side, double *addressOfArea)
{
    // Compute area
    double result = (side * side);

    // return result to pointer
    *addressOfArea = result;
}

void computePentagon(double side, double *addressOfArea)
{
    // Compute area
    double result = (1.720477 * side * side);

    // return result to pointer
    *addressOfArea = result;
}

int main (int argc, char *argv[])
{
    static double input[4];
    static double output[4];

    /* check if there are 4 inputs */
    if (argc != 5) {
	   printf("usage: ./P1 <double> <double> <double> <double>\n");
	   return EXIT_FAILURE;
	}
    /* change from String to double */
    for (int i = 0; i < 4; i++)
	input[i] = atof(argv[i+1]);

    // Call functions 
    computeCircle(input[0], &output[0]);
    computeTriangle(input[1], &output[1]);
    computeSquare(input[2], &output[2]);
    computePentagon(input[3], &output[3]);

    // Print dat 
    printf("CIRCLE, radius = %.5f, area = %.5f.\n", input[0], output[0]);
    printf("TRIANGLE, length = %.5f, area = %.5f.\n", input[1], output[1]);
    printf("SQUARE, length = %.5f, area = %.5f.\n", input[2], output[2]);
    printf("PENTAGON, length = %.5f, area = %.5f.\n", input[3], output[3]);

    return EXIT_SUCCESS;
}


	
    
