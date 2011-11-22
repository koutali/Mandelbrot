/*
 * visual.cpp
 *
 *  Created on: Apr 24, 2010
 *      Author: senem
 * 
 *  Description: all allegro related stuff goes here
 */

#include "visual.h"

/* initialize the allegro library */
bool setupAllegro()
{
	/* macro which initialises the allegro library */
	bool ret_bool = true;
	
	if(allegro_init() != 0)
	{
 		printf("allegro_init failed\n");
 		ret_bool = false;
    }
	else
	{
		if(install_keyboard() != 0)
		{
			printf("install_keyboard failed\n");
			ret_bool = false;
		}
		else
		{
		    /* set pixel format */
		    set_color_depth(COLOR_DEPTH);

		    /* switch into graphics mode */
		    int ret_int = set_gfx_mode(GFX_AUTODETECT_WINDOWED,
								    WIDTH,
								    HEIGHT,
								    VIRTUAL_SCREEN_WIDTH,
								    VIRTUAL_SCREEN_HEIGHT);

		    if(ret_int != 0)
		    {
			    printf("set_gfx_mode failed\n");
			    ret_bool = false;
		    }
		}
	}

	return ret_bool;
}

/* create a bmp file to be displayed later */
bool createMandelBrotBmp(char * bmp_file_name, BITMAP *bmp)
{
    bool ret_bool = true;

    /* save file */
    PALETTE palette;
    get_palette(palette);

    int ret_int = save_bitmap(bmp_file_name, bmp, palette);
    if(ret_int != 0)
    {
        printf("Bmp file generation failed.\n");
        ret_bool = false;
    }
    
    // will be done in the cleanup function [24.04.10, SBe]
    //destroy_bitmap(bmp);
    
    return ret_bool;
}

/* display a file, given with the name */
bool displayMandelBrotBmp(char * bmp_file_name, int dest_x, int dest_y, int num_bmps)
{
    bool ret_bool = true;
    
    BITMAP *bmp = load_bitmap(bmp_file_name, NULL);
    if(bmp == NULL)
    {
        printf("Bmp file loading failed.\n");
        ret_bool = false;
    }
    else
    {
        /* display on screen */
        //blit(bmp, screen, 0, 0, 0, 0, bmp->w, bmp->h);
        int width  =  WIDTH  / 4;
        int height =  HEIGHT / 4;
        
        stretch_blit(bmp,screen, 0, 0, bmp->w, bmp->h, dest_x, dest_y, width , height);

        /* delete figure */
        destroy_bitmap(bmp);
    }
    
    return ret_bool;
}
