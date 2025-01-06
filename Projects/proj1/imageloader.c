/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				PainJoker
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
	FILE *fp = fopen(filename, "r");
	if(!fp) {
		printf("Failed to open file %s.\n", filename);
		exit(0);
	}
	
	char image_type[3];
	fscanf(fp, "%s", image_type);
	int8_t is_not_p3 = strcmp("P3", image_type);
	if(is_not_p3) {
		printf("Wrong image type, only P3 permitted.\n");
		exit(0);
	}

	uint32_t row_num, col_num;
	fscanf(fp, "%u %u", &col_num, &row_num);
	uint8_t max_value;
	fscanf(fp, "%hhu", &max_value);

	Image *img = (Image *) malloc(sizeof(Image));
	if(!img) {
		printf("Memory allocate to img failed.\n");
		exit(0);
	}
	img->image = (Color **) malloc(sizeof(Color *) * row_num);
	if(!img->image) {
		printf("Memory allocate to img->image failed.\n");
		exit(0);
	}

	img->rows = row_num;
	img->cols = col_num;
	for(uint32_t row = 0; row < row_num; row++) {
		img->image[row] = (Color *) malloc(sizeof(Color) * col_num);
		if(!img->image[row]) {
			printf("Memory allocate to img->image[%d] failed.\n", row);
			exit(0);
		}
		for(uint32_t col = 0; col < col_num; col++) {
			Color color;
			fscanf(fp, "%hhu %hhu %hhu", &color.R, &color.G, &color.B);
			img->image[row][col] = color;
		}
	}

	fclose(fp);
	return img;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	uint32_t col_num, row_num;
	col_num = image->cols;
	row_num = image->rows;

	printf("P3\n%d %d\n255\n", col_num, row_num);
	for (uint32_t i = 0; i < row_num; i++) {
		for(uint32_t j = 0; j < col_num - 1; j++) {
			printf("%3hhu %3hhu %3hhu   ", 
				image->image[i][j].R, image->image[i][j].G, image->image[i][j].B);
		}
		printf("%3hhu %3hhu %3hhu\n", 
			image->image[i][col_num - 1].R, image->image[i][col_num - 1].G, image->image[i][col_num - 1].B);
	}
}

//Frees an image
void freeImage(Image *image)
{
	for(uint32_t i = 0; i < image->rows; i++) {
		free(image->image[i]);
	}
	free(image->image);
	free(image);
}