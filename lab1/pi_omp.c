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
        return ((double)time.tv_sec * (double)1e6 + (double)time.tv_usec);
}

#define START_COUNT_TIME stamp = getusec_();
#define STOP_COUNT_TIME(_m) stamp = getusec_() - stamp;\
                        stamp = stamp/1e6;\
                        printf ("%s%0.6f\n",(_m), stamp);

int main(int argc, char *argv[]) {
    double stamp;
    double x, sum=0.0, pi=0.0;
    double step;
    START_COUNT_TIME;

    const char Usage[] = "Usage: pi <num_steps> (try 1000000000)\n";
    if (argc < 2) {
	fprintf(stderr, Usage);
	exit(1);
    }
    long int num_steps = atol(argv[1]);
    step = 1.0/(double) num_steps;

    /* do computation -- using all available threads */
    //omp_set_num_threads(8);
    #pragma omp parallel private(x) firstprivate(num_steps,step) reduction(+: sum) // num_threads(8)
    {
        long int myid = omp_get_thread_num();
        long int howmany = omp_get_num_threads();
        long int thread_steps = num_steps/howmany;
        long int i_start = thread_steps*myid;
        long int i_end= thread_steps*(myid+1);
        if (myid==howmany-1) i_end = num_steps;

        for (long int i=i_start; i<i_end; i++) {
            x = (i+0.5)*step;
            sum += 4.0/(1.0+x*x);
        }
    }
    pi = step * sum;

    /* print results */
    printf("Number pi after %ld iterations = %.15f\n", num_steps, pi);

    STOP_COUNT_TIME("Execution time (secs.): ");

    return EXIT_SUCCESS;
}