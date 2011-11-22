/*
 * visual.h
 *
 *  Created on: Apr 24, 2010
 *      Author: senem
 */

#ifndef VISUAL_H_
#define VISUAL_H_

#include <allegro.h>
#include <stdio.h>   /* printf, file operations */

const double WIDTH  = 1024;
const double HEIGHT = 768;

const int VIRTUAL_SCREEN_WIDTH  = 0;
const int VIRTUAL_SCREEN_HEIGHT = 0;

const int COLOR_DEPTH = 32;

bool setupAllegro();
bool displayMandelBrotBmp(char * bmp_file_name, int dest_x, int dest_y, int num_bmps);
bool createMandelBrotBmp(char * bmp_file_name, BITMAP *bmp);

#endif /* VISUAL_H_ */
