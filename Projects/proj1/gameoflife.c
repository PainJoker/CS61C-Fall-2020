/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				PainJoker
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

uint8_t* calAlivedNeibors(Image *image, uint32_t row, uint32_t col) {
	int8_t const offsets[3] = {-1, 0, 1};
	uint8_t *alived_neibors = (uint8_t *) calloc(sizeof(uint8_t), 3);

	for(uint8_t i = 0; i < 3; i++) {  // for every offset
		for(uint8_t j = 0; j < 3; j++) {
			if(offsets[i] == 0 && offsets[j] == 0) continue;

			uint32_t neibor_row = (row + offsets[i] + image->rows) % image->rows;
			uint32_t neibor_col = (col + offsets[j] + image->cols) % image->cols;

			Color neibor_pixel = image->image[neibor_row][neibor_col];

			uint8_t neibor_is_alive_r = (neibor_pixel.R == UINT8_MAX);
			uint8_t neibor_is_alive_g = (neibor_pixel.G == UINT8_MAX);
			uint8_t neibor_is_alive_b = (neibor_pixel.B == UINT8_MAX);

			if(neibor_is_alive_r) alived_neibors[0]++;
			if(neibor_is_alive_g) alived_neibors[1]++;
			if(neibor_is_alive_b) alived_neibors[2]++;
		}
	}

	return alived_neibors;
}

uint8_t decideAlive(uint8_t is_alive, uint8_t alived_neibors, uint32_t rule) {
	uint32_t const mask = 0x1FF;
	uint16_t alive_rule = (rule >> 9) & mask;
	uint16_t dead_rule = rule & mask;
	uint8_t will_alive = 0;

	will_alive = ((is_alive? alive_rule: dead_rule) >> alived_neibors) & 1;

	return will_alive;
}
//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
	Color *ret_val = (Color *) malloc(sizeof(Color));
	Color cpixel = image->image[row][col];

	uint8_t *alived_neibors = calAlivedNeibors(image, row, col);
	uint8_t alived_neibors_r = alived_neibors[0];
	uint8_t alived_neibors_g = alived_neibors[1];
	uint8_t alived_neibors_b = alived_neibors[2];
	free(alived_neibors);

	uint8_t is_alive_r = (cpixel.R == UINT8_MAX);
	uint8_t will_alive_r = decideAlive(is_alive_r, alived_neibors_r, rule);
	uint8_t is_alive_g = (cpixel.G == UINT8_MAX);
	uint8_t will_alive_g = decideAlive(is_alive_g, alived_neibors_g, rule);
	uint8_t is_alive_b = (cpixel.B == UINT8_MAX);
	uint8_t will_alive_b = decideAlive(is_alive_b, alived_neibors_b, rule);

	ret_val->R = will_alive_r? UINT8_MAX: 0;
	ret_val->G = will_alive_g? UINT8_MAX: 0;
	ret_val->B = will_alive_b? UINT8_MAX: 0;

	return ret_val;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
{
	Image *next_img = (Image *) malloc(sizeof(Image));
	next_img->cols = image->cols;
	next_img->rows = image->rows;

	uint32_t row_num = image->rows;
	uint32_t col_num = image->cols;
	next_img->image = (Color **) malloc(sizeof(Color *) * row_num);
	if(!next_img->image) {
		printf("Failed to allocate memory to next_img->image.\n");
		exit(0);
	}
	for(uint32_t i = 0; i < row_num; i++) {
		next_img->image[i] = (Color *) malloc(sizeof(Color) * col_num);
		if(!next_img->image[i]) {
			printf("Failed to allocate memory to next_img->image[i].\n");
			exit(0);
		}
		for(uint32_t j = 0; j < col_num; j++) {
			Color *piexl = evaluateOneCell(image, i, j, rule);
			next_img->image[i][j] = *piexl;
			free(piexl);
		}
	}

	return next_img;
}

void printErrorMsg(void) {
	printf("usage: ./gameOfLife filename rule\n");
	printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
	printf("rule is a hex number beginning with 0x; Life is 0x1808.");
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
	if(argc != 3) {
		printErrorMsg();
		return -1;
	}
	char *file_name = argv[1];

	uint32_t const max_rule_code = 0x3FFFF;
	uint32_t rule = strtol(argv[2], NULL, 16);
	if(rule > max_rule_code) {
		printErrorMsg();
		return -1;
	}


	Image *img = readData(file_name);
	Image *next_img = life(img, rule);
	writeData(next_img);

	freeImage(next_img);
	freeImage(img);

	return 0;
}
