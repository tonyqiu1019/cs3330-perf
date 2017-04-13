/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/*
 * Please fill in the following team struct
 */
team_t team = {
    "bomb68",             /* Team name */

    "Tong Qiu",           /* First member full name */
    "tq7bw@virginia.edu", /* First member email address */

    "Charlie Wu",         /* Second member full name (leave blank if none) */
    "jw7jb@virginia.edu"  /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/*
 * naive_rotate - The naive baseline version of rotate
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/*
 * rotate - Our versions of rotate by cache blocking
 */
char rotate_descr[] = "rotate: 8 by 1 block";
void rotate(int dim, pixel *src, pixel *dst)
{
    int i, j, k, l;
    int i_step = 8, j_step = 1;

    for (i = 0; i < dim; i += i_step) {
        for (j = 0; j < dim; j += j_step) {
            for (k = 0; k < i_step; k++) {
                for (l = 0; l < j_step; l++) {
                    dst[RIDX(dim-1-j-l, i+k, dim)]
                        = src[RIDX(i+k, j+l, dim)];
                }
            }
        }
    }
}

char rotate_4x2_descr[] = "rotate: 4 by 2 block";
void rotate_4x2(int dim, pixel *src, pixel *dst)
{
    int i, j, k, l;
    int i_step = 4, j_step = 2;

    for (i = 0; i < dim; i += i_step) {
        for (j = 0; j < dim; j += j_step) {
            for (k = 0; k < i_step; k++) {
                for (l = 0; l < j_step; l++) {
                    dst[RIDX(dim-1-j-l, i+k, dim)]
                        = src[RIDX(i+k, j+l, dim)];
                }
            }
        }
    }
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_rotate_functions()
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);
    /* ... Register additional test functions here */
    add_rotate_function(&rotate_4x2, rotate_4x2_descr);
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/*
 * initialize_pixel_sum - Initializes all fields of sum to 0
 */
static void initialize_pixel_sum(pixel_sum *sum)
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/*
 * accumulate_sum - Accumulates field values of p in corresponding
 * fields of sum
 */
static void accumulate_sum(pixel_sum *sum, pixel p)
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/*
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum)
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/*
 * avg - Returns averaged pixel value at (i,j)
 */
static pixel avg(int dim, int i, int j, pixel *src)
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++)
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++)
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Another version of smooth.
 */

char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst)
{
    int i, j, red, green, blue;


    j = 0;

    red = src[RIDX(0, j, dim)].red + src[RIDX(1, j, dim)].red
        + src[RIDX(0, j+1, dim)].red + src[RIDX(1, j+1, dim)].red;
    green = src[RIDX(0, j, dim)].green + src[RIDX(1, j, dim)].green
        + src[RIDX(0, j+1, dim)].green + src[RIDX(1, j+1, dim)].green;
    blue = src[RIDX(0, j, dim)].blue + src[RIDX(1, j, dim)].blue
        + src[RIDX(0, j+1, dim)].blue + src[RIDX(1, j+1, dim)].blue;

    dst[RIDX(0, j, dim)].red = red / 4;
    dst[RIDX(0, j, dim)].green = green / 4;
    dst[RIDX(0, j, dim)].blue = blue / 4;

    for (i = 1; i < dim-1; i++) {
        red += src[RIDX(i+1, j, dim)].red + src[RIDX(i+1, j+1, dim)].red;
        green += src[RIDX(i+1, j, dim)].green + src[RIDX(i+1, j+1, dim)].green;
        blue += src[RIDX(i+1, j, dim)].blue + src[RIDX(i+1, j+1, dim)].blue;

        dst[RIDX(i, j, dim)].red = red / 6;
        dst[RIDX(i, j, dim)].green = green / 6;
        dst[RIDX(i, j, dim)].blue = blue / 6;

        red -= src[RIDX(i-1, j, dim)].red + src[RIDX(i-1, j+1, dim)].red;
        green -= src[RIDX(i-1, j, dim)].green + src[RIDX(i-1, j+1, dim)].green;
        blue -= src[RIDX(i-1, j, dim)].blue + src[RIDX(i-1, j+1, dim)].blue;
    }

    dst[RIDX(dim-1, j, dim)].red = red / 4;
    dst[RIDX(dim-1, j, dim)].green = green / 4;
    dst[RIDX(dim-1, j, dim)].blue = blue / 4;


    for (j = 1; j < dim-1; j++) {
        red = src[RIDX(0, j-1, dim)].red + src[RIDX(1, j-1, dim)].red
            + src[RIDX(0, j, dim)].red + src[RIDX(1, j, dim)].red
            + src[RIDX(0, j+1, dim)].red + src[RIDX(1, j+1, dim)].red;

        green = src[RIDX(0, j-1, dim)].green + src[RIDX(1, j-1, dim)].green
            + src[RIDX(0, j, dim)].green + src[RIDX(1, j, dim)].green
            + src[RIDX(0, j+1, dim)].green + src[RIDX(1, j+1, dim)].green;

        blue = src[RIDX(0, j-1, dim)].blue + src[RIDX(1, j-1, dim)].blue
            + src[RIDX(0, j, dim)].blue + src[RIDX(1, j, dim)].blue
            + src[RIDX(0, j+1, dim)].blue + src[RIDX(1, j+1, dim)].blue;

        dst[RIDX(0, j, dim)].red = red / 6;
        dst[RIDX(0, j, dim)].green = green / 6;
        dst[RIDX(0, j, dim)].blue = blue / 6;

        for (i = 1; i < dim-1; i++) {
            red += src[RIDX(i+1, j-1, dim)].red + src[RIDX(i+1, j, dim)].red
                + src[RIDX(i+1, j+1, dim)].red;
            green += src[RIDX(i+1, j-1, dim)].green + src[RIDX(i+1, j, dim)].green
                + src[RIDX(i+1, j+1, dim)].green;
            blue += src[RIDX(i+1, j-1, dim)].blue + src[RIDX(i+1, j, dim)].blue
                + src[RIDX(i+1, j+1, dim)].blue;

            dst[RIDX(i, j, dim)].red = red / 9;
            dst[RIDX(i, j, dim)].green = green / 9;
            dst[RIDX(i, j, dim)].blue = blue / 9;

            red -= src[RIDX(i-1, j-1, dim)].red + src[RIDX(i-1, j, dim)].red
                + src[RIDX(i-1, j+1, dim)].red;
            green -= src[RIDX(i-1, j-1, dim)].green + src[RIDX(i-1, j, dim)].green
                + src[RIDX(i-1, j+1, dim)].green;
            blue -= src[RIDX(i-1, j-1, dim)].blue + src[RIDX(i-1, j, dim)].blue
                + src[RIDX(i-1, j+1, dim)].blue;
        }

        dst[RIDX(dim-1, j, dim)].red = red / 6;
        dst[RIDX(dim-1, j, dim)].green = green / 6;
        dst[RIDX(dim-1, j, dim)].blue = blue / 6;
    }


    j = dim - 1;

    red = src[RIDX(0, j-1, dim)].red + src[RIDX(1, j-1, dim)].red
        + src[RIDX(0, j, dim)].red + src[RIDX(1, j, dim)].red;
    green = src[RIDX(0, j-1, dim)].green + src[RIDX(1, j-1, dim)].green
        + src[RIDX(0, j, dim)].green + src[RIDX(1, j, dim)].green;
    blue = src[RIDX(0, j-1, dim)].blue + src[RIDX(1, j-1, dim)].blue
        + src[RIDX(0, j, dim)].blue + src[RIDX(1, j, dim)].blue;

    dst[RIDX(0, j, dim)].red = red / 4;
    dst[RIDX(0, j, dim)].green = green / 4;
    dst[RIDX(0, j, dim)].blue = blue / 4;

    for (i = 1; i < dim-1; i++) {
        red += src[RIDX(i+1, j-1, dim)].red + src[RIDX(i+1, j, dim)].red;
        green += src[RIDX(i+1, j-1, dim)].green + src[RIDX(i+1, j, dim)].green;
        blue += src[RIDX(i+1, j-1, dim)].blue + src[RIDX(i+1, j, dim)].blue;

        dst[RIDX(i, j, dim)].red = red / 6;
        dst[RIDX(i, j, dim)].green = green / 6;
        dst[RIDX(i, j, dim)].blue = blue / 6;

        red -= src[RIDX(i-1, j-1, dim)].red + src[RIDX(i-1, j, dim)].red;
        green -= src[RIDX(i-1, j-1, dim)].green + src[RIDX(i-1, j, dim)].green;
        blue -= src[RIDX(i-1, j-1, dim)].blue + src[RIDX(i-1, j, dim)].blue;
    }

    dst[RIDX(dim-1, j, dim)].red = red / 4;
    dst[RIDX(dim-1, j, dim)].green = green / 4;
    dst[RIDX(dim-1, j, dim)].blue = blue / 4;
}

/*********************************************************************
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    add_smooth_function(&smooth, smooth_descr);
    /* ... Register additional test functions here */
}
