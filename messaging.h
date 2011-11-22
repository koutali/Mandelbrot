/*
 * messaging.h
 *
 *  Created on: Apr 24, 2010
 *      Author: senem
 */

#ifndef MESSAGING_H_
#define MESSAGING_H_

#include <time.h>    /* time */
#include <stdlib.h>  /* rand */
#include <stdio.h>   /* printf, file operations */
#include <string.h>  /* memset */

static char file_name[] = "log.txt";
static char file_mode[] = "w";

static char start_message[]      = "program started";
static char resource_message[]   = "resources created";
static char gen_end_message[]	 = "figure generation complete";
static char terminate_message[]  = "program terminated";
static char gen_start_message[]  = "figure generation started";

static char err_log_creation[] = "The log file cannot be created. The application will be terminated.\n";
static char err_allegro_init[] = "Allegro library cannot be instantiated. The application will be terminated.\n";
static char err_mutex_creation[] = "File lock cannot be created. Application will be terminated.\n";
static char err_thread_creation[] = "Mandelbrot thread cannot be created. Application will be terminated.\n";
static char err_bmp_creation[] = "Bitmap file cannot be created for thread. Application will be terminated.\n";

const int TIME_STRING_SIZE = 256;
static char time_string[TIME_STRING_SIZE]={0};

const int LOG_MESSAGE_SIZE = 256;
static char log_message[LOG_MESSAGE_SIZE] = {0};

/* helper functions for logging functionality */
char * getCurrentTime();
char * createMessageForLogFile(const char *current_time, const int iD, const char *message);

#endif /* MESSAGING_H_ */
