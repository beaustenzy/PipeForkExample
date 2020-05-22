/* File:     PipeForkExample.c
 *
 * Purpose:  Summates n count hyperbolic tangents among n count children
 *
 * Compile:  gcc -o PipeForkExample PipeForkExample.c -lm
 *
 * Note:     rand() slows down the calculations a bit, so times may differ from a traditional set input program
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h> 
#include <math.h>
#include "timer.h"
long double tanhCalc(void) {
	srand(time(NULL) ^ getpid()); //Sets the starting point of RNG; time(NULL) ^ getpid() ensures each child has unique sequence
	long double ctotal = 0;
	long long n = 1000000L; // Number of calculations
    for (long long i = 0; i < n; i++) // Calculates the sum of n count hyperbolic tangents 
    {
		long double num;
        long double a = (long double)rand()/RAND_MAX;
		num = tanh(a);
		//printf("Generated random num is %f and calculated tangent is %Lf\n", a, num );
        ctotal = ctotal + num ;
    }
    return ctotal;
}
int main(void) {
	double start, finish;
	GET_TIME(start);
	double start_a, finish_a;
	int n = 8; //Number of child processes
	int fd[2 * n];
	for (int i = 0; i < n; i++) {
		pipe(&fd[2*i]);
	}
    for (int i = 0; i < n; i++) { // Forks n count child processes
        int pid = fork(); 
		GET_TIME(start_a);
        if (pid == 0){ // If child, calculate n count tangents, then writes said sum to pipe buffer
            long double t = tanhCalc();
			close(fd[2*i]);
			write(fd[2*i + 1], &t, sizeof(t));
			GET_TIME(finish_a);
			printf("Elapsed time for child %d = %e seconds\nTotal generated for %d: %Lf\n",i, finish_a - start_a
			,i, t);
			close(fd[2*i + 1]); // This is probably redundant
            exit(0);
        }
    }
	long double b;
    long double total = 0;
    for (int i = 0; i < n; i++) { // Reads from pipe buffer until empty
		close(fd[2*i + 1]);
		read(fd[2*i], &b, sizeof(b));
		total = total + b; // Add total from one child
		close(fd[2*i]);
    }
	GET_TIME(finish);
	sleep(0.000025); // Helps ensure these two print statements print at the end; doesn't change the output, just order
    printf("Total generated from all children %Lf\n", total);
	printf("Elapsed time for parent = %e seconds\n", finish - start);
    return 0;
}
