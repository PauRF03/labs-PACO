/*
 * Compute pi by approximating the area under the curve f(x) = 4 / (1 + x*x)
 * between 0 and 1.
 *
 * Parallel version using OpenMP
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>	/* OpenMP */

double getusec_() {
        struct timeval time;
        gettimeofday(&time, NULL);
        return ((double)(time.tv_sec * 1000000L + time.tv_usec));
}

#ifndef NUMITERS
#define NUMITERS 1
#endif

double difference (long int num_steps, int n_threads){
    double x, sum=0.0;
    double pi1=0.0, pi2=0.0;
    double step = 1.0/(double) num_steps;

    double stamp1=getusec_();
    for (int iter=0; iter<NUMITERS ; iter++) {
        sum = 0.0;
        for (long int i=0; i<num_steps; ++i) {
            x = (i+0.5)*step;
            sum += 4.0/(1.0+x*x);
            }
        pi1 += step * sum;
        }
    stamp1=getusec_()-stamp1;

    omp_set_num_threads(n_threads);
    double stamp2=getusec_();
    for (int iter=0; iter<NUMITERS ; iter++) {
        if (n_threads>1) {
          sum = 0.0;
          #pragma omp parallel private(x)
          {
	        int myid = omp_get_thread_num();
	        int howmany = omp_get_num_threads();
            for (long int i=myid; i<num_steps; i+=howmany) {
                x = (i+0.5)*step;
           		#pragma omp critical
                sum += 4.0/(1.0+x*x);
                }
          }
        }
        else
        {
            sum = 0.0;
            for (long int i=0; i<num_steps; ++i) {
                x = (i+0.5)*step;
           		#pragma omp critical
                sum += 4.0/(1.0+x*x);
            }
        }
        pi2 += step * sum;
	}
    stamp2=getusec_()-stamp2;
    printf("pi: %f pi2:%f\n", pi1, pi2);
    printf("Total execution time for %d repetitions of pi, when executed with %ld iterations on %d threads: %.4f microseconds\n", NUMITERS, num_steps, n_threads, stamp2);
    return((stamp2-(stamp1/n_threads))/NUMITERS);
}

int main(int argc, char *argv[]) {
    const char Usage[] = "Usage: pi_omp_critical <num_steps> <num_threads>\n";
    if (argc < 3) {
	fprintf(stderr, Usage);
	exit(1);
    }
    long int num_steps = atol(argv[1]);
    int num_threads = atoi(argv[2]);

    double tmp = difference(num_steps, num_threads);
    printf("Total overhead, per repetition, when executed with %ld iterations on %d threads: %.4f microseconds\n", num_steps, num_threads, tmp);

    return EXIT_SUCCESS;
}
