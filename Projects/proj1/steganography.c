/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				PainJoker
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
	Color pixel = image->image[row][col];
	Color *new_color = (Color *) malloc(sizeof(Color));

	uint8_t assigned_value;
	uint8_t is_white = pixel.B % 2;
	if(is_white) {
		assigned_value = UINT8_MAX;
	} else {
		assigned_value = 0;
	}

	new_color->R = assigned_value;
	new_color->G = assigned_value;
	new_color->B = assigned_value;

	return new_color;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
	Image *msg = (Image *) malloc(sizeof(Image));
	msg->cols = image->cols;
	msg->rows = image->rows;

	uint32_t row_num = image->rows;
	uint32_t col_num = image->cols;
	msg->image = (Color **) malloc(sizeof(Color *) * row_num);
	if(!msg->image) {
		printf("Failed to allocate memory to msg->image.\n");
		exit(0);
	}
	for(uint32_t i = 0; i < row_num; i++) {
		msg->image[i] = (Color *) malloc(sizeof(Color) * col_num);
		if(!msg->image[i]) {
			printf("Failed to allocate memory to msg->image[i].\n");
			exit(0);
		}
		for(uint32_t j = 0; j < col_num; j++) {
			Color *piexl = evaluateOnePixel(image, i, j);
			msg->image[i][j] = *piexl;
			free(piexl);
		}
	}

	return msg;
}

/*
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("Wrong number of parameters, should given ppm file only.\n");
		return -1;
	}

	char *file_name = argv[1];
	Image *img = readData(file_name);
	Image *msg = steganography(img);
	writeData(msg);

	freeImage(msg);
	freeImage(img);

	return 0;
}
