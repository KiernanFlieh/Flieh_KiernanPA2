/*
 * PA2 Part A  --  tsum: one job, many threads.
 *
 * The program does its work in two phases:
 *
 *   Phase 1 (serial)    fill x[i] = sin(i).  You may NOT thread this phase.
 *                       It is here on purpose: it is the serial fraction that
 *                       Amdahl's law is about, and Part D asks you to measure it.
 *
 *   Phase 2 (parallel)  compute the sum of work(x[i]) using T threads.
 *
 * Thread k owns the half-open range [lo, hi) and writes its partial sum into
 * ITS OWN slot.  No two threads touch the same memory, so there is no shared
 * counter and you need no mutex, no semaphore and no condition variable.
 * Partitioning the data instead of locking it is the whole point of Part A.
 *
 * Build:  make
 * Run:    ./tsum --threads 4
 *         ./tsum --threads 4 --n 20000000 --reps 3
 */
#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + 1e-9 * (double)ts.tv_nsec;
}

/* The per-element work of phase 2.  Do NOT change this function.  The grader
 * compares your total against the reference, and any change here changes it. */
static double work(double v)
{
    double a = sqrt(fabs(v));
    a += sin(a) * cos(v);
    a += log1p(fabs(v)) * 0.5;
    a += atan(a);
    return a;
}

/* The argument struct handed to each thread.  Every thread gets its OWN
 * struct.  Handing all threads a pointer to the same object, or a pointer to
 * the loop counter, is Bug 1 in Part B. */
typedef struct {
    const double *x;
    long   lo;        /* first index this thread owns          */
    long   hi;        /* one past the last index it owns       */
    double partial;   /* this thread writes here and nowhere else */
} targ_t;

static void *sum_range(void *arg)
{
    //Grab the args passed to the thread. 
    targ_t *targ = (targ_t *)arg;
    //Sets the sum of  this thread to 0.0.
    double sum = 0.0;
    //Loops through the parts of the array assigned to the thread.
    for (long i = targ->lo; i < targ->hi; i++) {
        sum += work(targ->x[i]);
    }
    //Collect the final sum for the thread and store it. 
    targ->partial = sum;
    return NULL;
}

/* Runs both phases once.  Returns the total through *total_out and the two
 * phase times through *fill_out and *sum_out. */
static int run_once(double *x, long n, int nthreads,
                    double *fill_out, double *sum_out, double *total_out)
{
    double t0 = now_sec();
    for (long i = 0; i < n; i++) x[i] = sin((double)i);
    *fill_out = now_sec() - t0;

    double t1 = now_sec();

    double total = 0.0;

    //Stores the thread IDs for each thread.
    pthread_t *threads = malloc(nthreads * sizeof(pthread_t));
    //Create an array of targ_t structs for each thread.
    targ_t *args = malloc(nthreads * sizeof(targ_t));

    //Checking to see if the memory allocation failed for threads or args.
    if (threads == NULL || args == NULL) {
        fprintf(stderr, "tsum: cannot allocate memory for threads or args\n");

        //Free memory that was allocated. 
        free(threads);
        free(args);
        return 1;
    }

    //Loop through and assign each thread its own part of the array. 
    for (int k = 0; k < nthreads; k++) {
        args[k].x = x;
        args[k].lo = (long)k * n / nthreads;
        args[k].hi = (long)(k + 1) * n / nthreads;
        args[k].partial = 0.0;
        
        //Create the thread and pass it the args for its part of the array.
        int rc = pthread_create(&threads[k], NULL, sum_range, &args[k]);
        //Checks to see if thread failed to create.
        if (rc != 0) {
            fprintf(stderr, "tsum: pthread_create failed: %s\n", strerror(rc));
            for (int j = 0; j < k; j++) {
                pthread_join(threads[j], NULL);
            }

            //Free memory that was allocated.
            free(threads);
            free(args);
            return 1;
        }
    }
    //Var for keeping track of join errors.
    int join_error = 0;

    //Waiting for each thread to finish.
    for (int k = 0; k < nthreads; k++) {
        int rc = pthread_join(threads[k], NULL);
        //Checking to see if thread failed to join.
        if (rc != 0) {
            fprintf(stderr, "tsum: pthread_join failed: %s\n", strerror(rc));
            join_error = 1;
        }
    }

    //Exit if there was a join error.
    if (join_error) {
        free(threads);
        free(args);
        return 1;
    }
    //Adding up all the partial sums to get total.
    for (int k = 0; k < nthreads; k++) {
        total += args[k].partial;
    }
    //Free memory that was allocated.
    free(threads);
    free(args);

    *sum_out = now_sec() - t1;
    *total_out = total;
    return 0;
}

int main(int argc, char **argv) 
{
    long n = 20000000;
    int nthreads = 1, reps = 3;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--threads") && i + 1 < argc)    nthreads = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--n") && i + 1 < argc)     n        = atol(argv[++i]);
        else if (!strcmp(argv[i], "--reps") && i + 1 < argc)  reps     = atoi(argv[++i]);
        else { fprintf(stderr, "usage: %s [--threads T] [--n N] [--reps R]\n", argv[0]); return 2; }
    }
    if (nthreads < 1 || n < 1 || reps < 1) {
        fprintf(stderr, "tsum: --threads, --n and --reps must all be >= 1\n");
        return 2;
    }

    double *x = malloc((size_t)n * sizeof *x);
    if (!x) { fprintf(stderr, "tsum: cannot allocate %ld doubles\n", n); return 1; }

    /* Keep the FASTEST COMPLETE REPETITION, not the fastest fill and the
     * fastest sum from different repetitions -- those two minima can come from
     * different runs, and their sum is then a time no run actually took. */
    double best_wall = 0.0, best_fill = 0.0, best_sum = 0.0, total = 0.0;
    for (int r = 0; r < reps; r++) {
        double f, s, t;
        if (run_once(x, n, nthreads, &f, &s, &t) != 0) { free(x); return 1; }
        double wall = f + s;
        if (r == 0 || wall < best_wall) {
            best_wall = wall; best_fill = f; best_sum = s; total = t;
        }
    }

    /* The fastest of several runs, not the average: a slow run means something
     * else was using the CPU, which is noise, not a property of your code. */
    printf("threads   %d\n", nthreads);
    printf("n         %ld\n", n);
    printf("reps      %d\n", reps);
    printf("total     %.6f\n", total);
    printf("fill_s    %.4f\n", best_fill);
    printf("sum_s     %.4f\n", best_sum);
    printf("wall_s    %.4f\n", best_wall);

    free(x);
    return 0;
}
