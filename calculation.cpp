/*
 * calculation.cpp
 *
 *  Created on: Apr 24, 2010
 *      Author: senem
 */
 
#include "calculation.h"
#include "messaging.h"
#include "visual.h"

/* main process */
int main()
{
	/* create log file - has to be done here, since main process needs to log itself too.
	 * if file cannot be created, terminate the application
	 */
	FILE * log_file = fopen(file_name, file_mode);
	char * buffer   = NULL;

	int pid = getpid();

	if(log_file == NULL)
	{
	    fputs(err_log_creation, log_file);
	    printf("%s\n", err_log_creation);
	}
	else if(!setupAllegro())
	{
	    fputs(err_allegro_init, log_file);
	    printf("%s\n", err_allegro_init);
	}
	else
	{
		/* create start message for the main process and store it in the log file */
		buffer = createMessageForLogFile(getCurrentTime(), pid, start_message);
		fputs(buffer, log_file);
		printf("%s\n", buffer);

		/* no dynamic allocation, since number of threads is known in advance */

		pthread_t thread_ids[MAX_NUM_THREADS];
		memset(thread_ids, 0, MAX_NUM_THREADS * sizeof(pthread_t));

		thread_variables tvars[MAX_NUM_THREADS];
		memset(tvars, 0, MAX_NUM_THREADS * sizeof(thread_variables));

		if(createResources(log_file, thread_ids, tvars))
		    window_handler(tvars);
	    else /* in successfull case threads will have terminated themselves; thus no need to send an explicit message */
            terminateMandelbrotThreads(thread_ids);

	    /* log termination */
	    buffer = createMessageForLogFile(getCurrentTime(), pid, terminate_message);
	    fputs(buffer, log_file);
	    printf("%s\n", buffer);    
	            
        cleanup(log_file, tvars); 
	}

	return EXIT_SUCCESS;
}
END_OF_MAIN()

/* helper functions for handling resource management */
bool createResources(FILE *log_file, pthread_t *thread_ids, thread_variables *tvars)
{
	/* create the shared mutex for the threads
	 * if mutex cannot be created, terminate application
	 */
	bool ret_bool = true;
	
	pthread_mutex_t file_lock;
	int err_num = pthread_mutex_init(&file_lock, NULL);
	if(err_num != 0)
	{
		fputs(err_mutex_creation, log_file);
		printf("%s\n", err_mutex_creation);
		
		ret_bool = false;
	}
	else
	{
		bool resources_created = true;

		srand(time(NULL));

		/* create mandelbrot threads */
		for(int i = 0; i < MAX_NUM_THREADS && resources_created; i++)
		{
			/* create mandelbrot thread variables */
			tvars[i].log_file = log_file;
			tvars[i].lock     = &file_lock;
			
			tvars[i].thread_number    = i + 1;
			tvars[i].iteration_number = (rand() % ITERATIONS) + 1;
			
			strcpy(tvars[i].bmp_name, createBmpName(tvars[i].thread_number));
			
            tvars[i].bmp = create_bitmap(WIDTH, HEIGHT);
            if(tvars[i].bmp == NULL)
            {
                fputs(err_bmp_creation, log_file);
                printf("%s\n", err_bmp_creation);
                
                ret_bool = false;
				resources_created = false;
            }
            else
            {                           
			    int err_num = pthread_create(&thread_ids[i], NULL, mandelbrot_thread, &tvars[i]);
			    if(err_num != 0)
			    {
		            fputs(err_thread_creation, log_file);
		            printf("%s\n", err_thread_creation);
		            
		            ret_bool = false;
				    resources_created = false;
			    }
			}
		} // end of thread creation block

		/* we wait until mandelbrot threads have terminated. since that indicates that
		 * we are done generating the figures, we display the figures and then terminate.
		 * [17.04.10, SBe]
		 */
		if(resources_created)
		{	
			int pid = getpid();

            /* wait for thread termination */
			for(int i = 0; i < MAX_NUM_THREADS;i++)
				pthread_join(thread_ids[i], NULL);

			/* report that figure generation is complete */
			char * buffer = createMessageForLogFile(getCurrentTime(), pid, gen_end_message);
			fputs(buffer, log_file);
			printf("%s\n", buffer);
		}
	}
	
	return ret_bool;
}

void cleanUpMandelbrotThreads(thread_variables *tvars)
{
    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        if(tvars[i].bmp != NULL)
            destroy_bitmap(tvars[i].bmp);
    }
}

/* called upon error during mandelbrot thread creation
 * no need to carry our error checking for pthread_kill because it
 * doesn't make any sense to catch errors upon application shutdown
 */
void terminateMandelbrotThreads(pthread_t *thread_ids)
{
	/* quit signal */
	int kill_signal = 3;
	for(int i = 0; i < MAX_NUM_THREADS; i++)
	{
		if(thread_ids[i] != 0)
			pthread_kill(thread_ids[i], kill_signal);
	}
}

/* end of resource management functions */

/* thread functions */
/* mandelbrot threads. each thread will be called with  */
void * mandelbrot_thread(void * mandel_thread_vars)
{
	thread_variables * tvars = (thread_variables *)mandel_thread_vars;

	/* first call */
	if(tvars->num_calls == 0)
	{			
	    
        /* obtain lock to shared file */
        pthread_mutex_lock(tvars->lock);
            
		int thread_id = (int) pthread_self();
        
        printf("Thread[%d] with TID %d, iteration count: %d, name: %s\n", tvars->thread_number, thread_id, tvars->iteration_number, tvars->bmp_name);
            
		/* log start of figure generation */
		char * buffer = createMessageForLogFile(getCurrentTime(), thread_id, gen_start_message);
		fputs(buffer, tvars->log_file);

		printf("%s\n", buffer);
		
	    /* unlock mutex */
        pthread_mutex_unlock(tvars->lock);
	}

	/* do mandelbrot calculation here */
	generateMandelBrotFigure(tvars->bmp, tvars->iteration_number);
	
	tvars->num_calls++;
    
    /* save the bitmap as an bmp file */
    bool ret_bool = createMandelBrotBmp(tvars->bmp_name, tvars->bmp);

	/* to eliminate the warning of xcode "control reaches end of non-void function" */
	return 0;
}
/* end of thread functions */

/* window handler function */
const int NUM_VERTICAL_BMPS   = 2;
const int NUM_HORIZONTAL_BMPS = 4;

const int WIDTH_INCR  = WIDTH / NUM_HORIZONTAL_BMPS;
const int HEIGHT_INCR = HEIGHT / NUM_VERTICAL_BMPS;

void window_handler(thread_variables *tvars)
{
    int num_bmps = MAX_NUM_THREADS;
    
    int display_width  = 0;
    int display_height = 0;
    
    int mult_factor = 0;
    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        mult_factor   = i % NUM_HORIZONTAL_BMPS;
        display_width = mult_factor * WIDTH_INCR;
        
        if(i >= NUM_HORIZONTAL_BMPS)
            display_height = HEIGHT_INCR;
         
        displayMandelBrotBmp(tvars[i].bmp_name, display_width, display_height, num_bmps);
    }
    
    /* poll keyboard until user requests quit */
    bool quit = false;
    do
    {
        poll_keyboard();
        
        if(key[KEY_Q] || key[KEY_ESC])
            quit = true;
        
    }while(!quit);
}

void cleanup(FILE *log_file, thread_variables *tvars)
{
    cleanUpMandelbrotThreads(tvars);

    if(log_file != NULL)
    {
        fclose(log_file);
        log_file = NULL;
    }
}

void generateMandelBrotFigure(BITMAP *bmp, int iteration_count)
{
    for(int y = 0; y < HEIGHT; y++)
    {
        /* map the y coordinate of a given point to the imaginary part of a complex number */
        double c_im = MaxIm - y * Im_factor;
        
        for(int x = 0; x < WIDTH; x++)
        {
            /* map the x coordinate of a given point to the real part of a complex number */
            double c_re = MinRe + x * Re_factor;

            /* Z_re and Z_im denote next values of Z */
            double Z_re = c_re;
            double Z_im = c_im;
            
            for(int n = 0; n < iteration_count; n++)
            {    
                double Z_re2 = Z_re * Z_re;
                double Z_im2 = Z_im * Z_im;
                
                /* if the absolute value of Z ever gets bigger than 2, 
                 * it will never return to a place closer than 2, 
                 * but it will actually rapidly escape to infinity;
                 * thus the range check
                 */
                if(Z_re2 + Z_im2 > 4)
                {
                    putpixel(bmp, x, y, makecol(n * 10, n *20, 0));
                    break;
                }
                
                Z_im = 2 * Z_re * Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            } 
        }
    }
}


char * createBmpName(int thread_num)
{
    char bmp_file_name[] = "mandelbrot";
    char bmp_extension[] = ".bmp";

    memset(bmp_name, 0, BMP_NAME_SIZE);
    
    sprintf(bmp_name, "%s%d%s",bmp_file_name , thread_num, bmp_extension);

	return bmp_name;
    
}
