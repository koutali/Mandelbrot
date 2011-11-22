/*
 * calculation.h
 *
 *  Created on: Apr 24, 2010
 *      Author: senem
 */

#ifndef CALCULATION_H_
#define CALCULATION_H_

#include <stdio.h>   /* printf, file operations */
#include <stdlib.h>  /* rand */
#include <pthread.h> /* pthread and mutex definitions */
#include <signal.h>  /* pthread_kill */
#include <unistd.h>  /* getpid */
#include <string.h>  /* memset */
#include <allegro.h> /* allegro lib. include */

#include "visual.h"

const int ITERATIONS = 256;

/* origin of the calculation code taken from: http://warp.povusers.org/Mandelbrot/
 * minor modifications were made.
 */

 /* x-axis represents the real part of a complex number
 * y-axis represents the imaginary part of a complex number
 *
 * four edge points of the screen represent following complex numbers:
 *
 * upper left edge: (-2.0, 1.0)
 * lower left edge: (-2.0, -1.0)
 * upper right edge: (1.0, 1.0)
 * lower right edge: (1.0, -1.0)
 *
 * we derive line equation with the given four points in order to map the complex numbers
 * to the drawing canvas.
 */
const double MinRe = -2.0;
const double MaxRe = 1.0;
const double MinIm = -1.2;
const double MaxIm = MinIm + (MaxRe - MinRe) * HEIGHT / WIDTH;

/* x-slope and y-slope of the line equations */
const double Re_factor = (MaxRe - MinRe)/(WIDTH - 1);
const double Im_factor = (MaxIm - MinIm)/(HEIGHT - 1);

const int MAX_NUM_THREADS = 8;

const int BMP_NAME_SIZE = 256;
static char bmp_name[BMP_NAME_SIZE] = {0};

typedef struct
{
	FILE			*log_file;	/* handle to the shared log file */
	pthread_mutex_t *lock;		/* lock for mutual exclusion (in thread body) */

	int thread_number;			/* needed for the generation of the jpeg file names */
	int iteration_number;		/* needed for mandelbrot calculation */
	int num_calls;              /* number of calls so far */

	BITMAP *bmp;				/* where we'll store image info which will later
						         * be converted into the bitmap file */
    						         
    char bmp_name[BMP_NAME_SIZE];		         
}thread_variables;

bool createResources(FILE *log_file, pthread_t *thread_ids, thread_variables *tvars);
void * mandelbrot_thread(void * mandel_thread_vars);
void window_handler(thread_variables *tvars);
void terminateMandelbrotThreads(pthread_t *thread_ids);
void cleanUpMandelbrotThreads(thread_variables *tvars);
void cleanup(FILE *log_file, thread_variables *tvars);
void generateMandelBrotFigure(BITMAP *bmp, int iteration_count);
char * createBmpName(int thread_num);

#endif /* CALCULATION_H_ */
