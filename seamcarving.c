#include <stdio.h>
#include <math.h>
#include "seamcarving.h"
#include "c_img.h"
// compute the dual-gradient energy function, and place it in the struct rgb_img *grad.

void calc_energy(struct rgb_img *im, struct rgb_img **grad)
{

    int h = im->height;
    int w = im->width;

    create_img(grad, h, w);
    
    // Original RGB
    
    double red[h][w];
    double green[h][w];
    double blue[h][w];

    // Differences in red, green, and blue components along x-axis
    double rx[h][w];
    //rx[0][0] = 5; this is fine
    double gx[h][w];
    double bx[h][w];

    // X energy total : root((rx)^2 + (gx)^2 + (bx)^2)
    double xen[h][w];

    // Differences in red, green, and blue components along y-axis
    //printf("%d %d",h,w);
    double ry[h][w];
    //ry[0][0] = 5; //cant touch ry for some reason

    /*for (int col = 0; col <= w - 1; col++) // loop through the columns
    {
        for (int row = 0; row <= h - 1; row++) // loop through the rows
        {
            ry[row][col] = 0;
        }
    }*/

    double gy[h][w];
    double by[h][w];

    // Y energy total : root((ry)^2 + (gy)^2 + (by)^2)
    double yen[h][w];

    // Energy of the pixel
    double energy[h][w];


// ORIGINAL RGB ARRAYS
    for (int col = 0; col <= w - 1; col++) // loop through the columns
    {
        for (int row = 0; row <= h - 1; row++) // loop through the rows
        {
            red[row][col] = get_pixel(im, row, col, 0);
            green[row][col] = get_pixel(im, row, col, 1);
            blue[row][col] = get_pixel(im, row, col, 2);
            
        }
    }

// MAKE THE DIFFERENCE X and Y RGB ARRAYS
    for (int col = 0; col <= w-1; col++) // loop through the columns
    {

        for (int row = 0; row <= h-1; row++) // loop through the rows
        {
            
            // SET UP X DIFFERENCE ARRAYS

            if ((col + 1) > (w - 1))
            {
                //printf("working\n");
                rx[row][col] = red[row][0] - red[row][col - 1];
                bx[row][col] = blue[row][0] - blue[row][col - 1];
                gx[row][col] = green[row][0] - green[row][col - 1];

            } else if ((col - 1) < 0)

            {
                rx[row][col] = red[row][col + 1] - red[row][w - 1];
                bx[row][col] = blue[row][col + 1] - blue[row][w - 1];
                gx[row][col] = green[row][col + 1] - green[row][w - 1];

            } else
            {
                rx[row][col] = red[row][col + 1] - red[row][col - 1];
                bx[row][col] = blue[row][col + 1] - blue[row][col - 1];
                gx[row][col] = green[row][col + 1] - green[row][col - 1];
            }


            // SET UP Y DIFFERENCE ARRAYS

            //printf("Row: %d\n",row);
            //printf("Col: %d\n", col);

            if ((row + 1) > (h - 1))
            {
                
                //ry[row][col] = 50;
                //rx[row][col] = 50;

                //printf("%d and %d\n", row,col);
                //ry[0][0] = 10;
                //printf("%f\n",ry[0][0]);
                ry[row][col] = red[0][col] - red[row - 1][col]; //problematic
                by[row][col] = blue[0][col] - blue[row - 1][col]; //problematic
                gy[row][col] = green[0][col] - green[row - 1][col]; //problematic

            }

            else if ((row - 1) < 0)
            {
                ry[row][col] = red[row + 1][col] - red[h - 1][col]; //problematic
                by[row][col] = blue[row + 1][col] - blue[h - 1][col]; //problematic
                gy[row][col] = green[row + 1][col] - green[h - 1][col]; //problematic

            } else
            {
                ry[row][col] = red[row + 1][col] - red[row - 1][col]; //problematic
                by[row][col] = blue[row + 1][col] - blue[row - 1][col]; //problematic
                gy[row][col] = green[row + 1][col] - green[row - 1][col]; //problematic
            }

            // SET UP ENERGY ARRAY

    
            xen[row][col] = ((rx[row][col]) * (rx[row][col])) + ((gx[row][col]) * (gx[row][col])) + ((bx[row][col]) * (bx[row][col]));
            yen[row][col] = ((ry[row][col]) * (ry[row][col])) + ((gy[row][col]) * (gy[row][col])) + ((by[row][col]) * (by[row][col]));
            energy[row][col] = sqrt(xen[row][col] + yen[row][col]);
            energy[row][col] = (uint8_t) ((energy[row][col])/10);
            
            set_pixel(*grad, row, col, energy[row][col],energy[row][col],energy[row][col]);

        }
    }

}

void dynamic_seam(struct rgb_img *grad, double **best_arr)
{
    int h = grad->height;
    int w = grad->width;

    *best_arr = (double *)malloc(sizeof(double) * w * h);
    
    for (int row = 0; row <= h - 1; row++)
    {
        for (int col = 0; col <= w - 1; col++)
        {
            (*best_arr)[row * w + col] = get_pixel(grad,row,col,0);
        }
    }

    for (int row = 0; row <= h - 1; row++)
    {
        for (int col = 0; col <= w - 1; col++)
        {
            if (row == 0) // top row
            {
                (*best_arr)[row * w + col] = get_pixel(grad,row,col,0);

            } else if (col == 0)
            {

                if (((*best_arr)[(row-1) * w + col]) <= ((*best_arr)[(row-1) * w + (col+1)]))
                {
                    (*best_arr)[row * w + col] = (*best_arr)[row * w + col] + (*best_arr)[(row-1) * w + col];

                } else
                {
   
                    (*best_arr)[row * w + col] = (*best_arr)[row * w + col] + (*best_arr)[(row-1) * w + (col+1)];
                }
                
            } else if (col == w - 1)
            {
                if ((*best_arr)[(row - 1)* w + col] <= (*best_arr)[(row-1) * w + (col-1)])
                {
            
                    (*best_arr)[row * w + col] = (*best_arr)[row * w + col] + (*best_arr)[(row - 1)* w + col];

                } else
                {
                    (*best_arr)[row * w + col] = (*best_arr)[row * w + col] + (*best_arr)[(row-1) * w + (col-1)];
                }

            } else
            {

                if (((*best_arr)[(row-1) * w + (col-1)] <= (*best_arr)[(row-1) * w + col]) && ((*best_arr)[(row-1) * w + (col-1)] <= (*best_arr)[(row-1) * w + (col+1)]))
                {
                    
                    (*best_arr)[row * w + col] = (*best_arr)[row * w + col] + (*best_arr)[(row-1) * w + (col-1)];

                } 
                
                else if (((*best_arr)[(row-1) * w + col] <= (*best_arr)[(row-1) * w + (col-1)]) && ((*best_arr)[(row-1) * w + col] <= (*best_arr)[(row-1) * w + (col+1)]))
                {
                    (*best_arr)[row * w + col] = (*best_arr)[row * w + col] + (*best_arr)[(row-1) * w + col];

                } 
                else if (((*best_arr)[(row-1) * w + (col+1)] <= (*best_arr)[(row-1) * w + col]) && ((*best_arr)[(row-1) * w + (col+1)] <= (*best_arr)[(row-1) * w + (col-1)]))
                
                {
                    (*best_arr)[row * w + col] = (*best_arr)[row * w + col] + (*best_arr)[(row-1) * w + (col+1)];

                } 
            } 
                    
            //printf("%f at row %d\n",(*best_arr)[row * w + col], row);
        }
    }
}

void recover_path(double *best, int height, int width, int **path)
{    
    //printf("\nRecover Path\n");
    // Find the smallest value at the bottom "row"
    // Go upwards from there
    // loop through best's bottom row and find the smallest

    //printf("working here ");
    //printf("%d",height);

    *path = (int *)malloc(sizeof(int) * (height-1)); // making space for the best path

    //int row = 0;
    int low = best[(height - 1) * (width) + (width - 1)]; // store the lowest in here, temporarily it's the last one
    int lowest_col = width - 1; // temporarily

    for (int col = 0; col <= width - 1; col++)
    {
            //printf("Lowest Options: %f\n",best[(height - 1) * (width) + col]);

           if (best[(height - 1) * (width) + col] < low)
           {
               low = best[(height - 1) * (width) + col];
               //printf("NEW LOW: %f\n",best[(height - 1) * (width) + col]);
               //printf("NEW LOW: \n", low);
               lowest_col = col;
           }
    }
    (*path)[height - 1] = lowest_col; //put in the last step of the path
    //printf("first thing\n");
    //printf("%d\n",(*path)[height - 1]);
    //printf("Low is %d\n", low);

    int col = lowest_col; //for now
    //printf("The height is %d", height - 1);

    for (int row = height - 1; row >=1; row--) // loop through the rows starting from the last one
    {

            //printf("entering");
            if ((col - 1) < 0)
            {
                // choose from the one above the the one above and to the right
                if (best[(row - 1) * width + col] < best[(row - 1) * width + (col + 1)])
                {
                    lowest_col = col;
                }
                else
                {
                    lowest_col = col + 1;
                }

            }

            else if ((col + 1) > (width - 1))
            {
                // choose from the one above and to the left and the one above
                if (best[(row - 1) * width + (col-1)] < best[(row - 1) * width + col])
                {
                    lowest_col = col - 1;
                }
                else
                {
                    lowest_col = col;
                }

            }

            else
            {
                //printf("here\n");
                /*printf("One: %f\n",best[(row - 1) * width + (col - 1)]);
                printf("Two: %f\n",best[(row - 1) * width + (col)]);
                printf("Three: %f\n",best[(row - 1) * width + (col + 1)]);*/

                // choose from all three
                if ((best[(row - 1) * width + (col - 1)] <= best[(row - 1) * width + col]) && (best[(row - 1) * width + (col - 1)] <= best[(row - 1) * width + (col+1)]))
                {
                    lowest_col = col - 1;

                } else if ((best[(row - 1) * width + col] <= best[(row - 1) * width + (col - 1)]) && (best[(row - 1) * width + col] <= best[(row - 1) * width + (col+1)]))
                {
                    lowest_col = col;

                } else if ((best[(row - 1) * width + (col + 1)] <= best[(row - 1) * width + (col-1)]) && (best[(row - 1) * width + (col+1)] <= best[(row - 1) * width + col]))
                {
                    lowest_col = col + 1;
                }

            }

           // printf("One: %f\n",best[(row - 1) * width + (col - 1)]);
           // printf("Two: %f\n",best[(row - 1) * width + (col)]);
            //printf("Three: %f\n",best[(row - 1) * width + (col + 1)]);
           (*path)[row-1] = lowest_col; // keep adding steps to the path as it goes higher
           //printf("row being added: %d", row);
           //printf("\nPath Step: %d\n",(*path)[row]);
           col = lowest_col;
    }

    for (int i = 0; i <= height-1; i++)
    {
        //printf("%d\n",(*path)[i]);
    }


    
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path)
{
    // Original RGB
    //printf("Remove Seam\n");

    int h = src->height;
    int w = src->width;
    create_img(dest, h, w-1); // the new image will be one pixel less wide because of the seam removal
    
    
    double red[h][w];
    double green[h][w];
    double blue[h][w];

    // ORIGINAL RGB ARRAYS
    // skip getting for path values though

    for (int col = 0; col <= w - 1; col++) // loop through the columns
    {
        for (int row = 0; row <= h - 1; row++) // loop through the rows
        {

            /*if (col != path[row])
            {
                if (col == 2)
                {
                    printf("COLUMN 2 SUPPOSED TO GO %d",path[row]);
                }
                red[row][col] = get_pixel(src, row, col, 0);
                green[row][col] = get_pixel(src, row, col, 1);
                blue[row][col] = get_pixel(src, row, col, 2);
                printf("Keep at row %d and column %d, %d\n",row, col,path[row]);
            } else
            {
                //printf("At row %d and column %d, %d\n",row, col,path[row]);
                if (col >= path[row])
                {
                    red[row][col] = get_pixel(src, row, col + 1, 0);
                    green[row][col] = get_pixel(src, row, col + 1, 1);
                    blue[row][col] = get_pixel(src, row, col + 1, 2);
                } else 
                {
                    red[row][col] = get_pixel(src, row, col, 0);
                    green[row][col] = get_pixel(src, row, col, 1);
                    blue[row][col] = get_pixel(src, row, col, 2);
                }
            }*/


            
            if (col >= path[row])
            {
                red[row][col] = get_pixel(src, row, col + 1, 0);
                green[row][col] = get_pixel(src, row, col + 1, 1);
                blue[row][col] = get_pixel(src, row, col + 1, 2);
            } else 
            {
                red[row][col] = get_pixel(src, row, col, 0);
                green[row][col] = get_pixel(src, row, col, 1);
                blue[row][col] = get_pixel(src, row, col, 2);
            }
            

        }
    }

    for (int col = 0; col <= w - 1; col++) // loop through the columns
    {
        for (int row = 0; row <= h - 1; row++) // loop through the rows
        {
            set_pixel(*dest, row, col, red[row][col],green[row][col],blue[row][col]); 
        }
    }

    write_img(*dest, "six_removed12.bin");


}


//int main(void)
//{
    //printf("\nhello");
    /*struct rgb_img *ocean;
    struct rgb_img *gradient;

    read_in_img(&ocean, "HJoceanSmall.bin");
    printf("%zu\n",ocean->height);
    create_img(&ocean, ocean->height, ocean->width);

    read_in_img(gradient,"HJoceanSmall.bin");

    calc_energy(ocean, &gradient);*/
    //printf("hello");
    //printf("%s",*gradient);
   // write_img(gradient, "neww_ocean.bin");

    //double *arr;

    //dynamic_seam(gradient,&arr);

    /*struct rgb_img *three;
    struct rgb_img *gradthree;
    double *tharr;

    read_in_img(&three, "3x4.bin");
    calc_energy(three, &gradthree);
    print_grad(three);
    print_grad(gradthree);
    */


    /*struct rgb_img *six;
    struct rgb_img *gradsix;
    double *sixarr = (double *)malloc(sizeof(double) * 6 * 5);


    read_in_img(&six, "6x5.bin");
    calc_energy(six, &gradsix);
    //printf("GRADSIX\n");
    //print_grad(gradsix);

    printf("DYNAMIC SEAM\n");
    dynamic_seam(gradsix,&sixarr);

    for (int i = 0; i < 30; i++)
    {
        printf("\n%f", (sixarr)[i]);
    }

    //int *h = 5;
    //int *w = 6;

    int *p = (int *)malloc(sizeof(int) * 6 * 5);
    struct rgb_img *destination;
    recover_path(sixarr, 5, 6, &p);

    for (int i = 0; i <=5 ; i++)
    {
        printf("\n%d", (p)[i]);
    }

    remove_seam(six, &destination, p);
    */

    /*struct rgb_img *ocean;
    struct rgb_img *gradient;

    read_in_img(&ocean, "HJoceanSmall.bin");
    printf("%zu\n",ocean->height);
    create_img(&ocean, ocean->height, ocean->width);

    read_in_img(&gradient,"HJoceanSmall.bin");

    calc_energy(ocean, &gradient);*/

    /*struct rgb_img *three;
    struct rgb_img *gradthree;
    double *tharr;

    read_in_img(&three, "3x4.bin");
    calc_energy(three, &gradthree);
    print_grad(three);
    print_grad(gradthree);

    */

    //struct rgb_img *six;
    //struct rgb_img *gradsix;
    //double *sixarr = (double *)malloc(sizeof(double) * 6 * 5);


    //read_in_img(&six, "6x5.bin");
    //calc_energy(six, &gradsix);
    //printf("\nGRADSIX\n");
    //print_grad(gradsix);

    /*struct rgb_img *test;
    struct rbg_image *test_grad;

    read_in_img(&test, "HJoceanSmall.bin");
    calc_energy(test,&test_grad);
    print_grad(test_grad);*/


    /*struct rgb_img *six;
    struct rgb_img *gradsix;
    double *sixarr;


    read_in_img(&six, "6x5.bin");
    calc_energy(six, &gradsix);
    printf("GRADSIX\n");
    print_grad(gradsix);*/


    /*struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "HJoceanSmall.bin");
    
    for(int i = 0; i < 5; i++)
    {
        printf("i = %d\n", i);
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best);
        recover_path(best, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        char filename[200];
        sprintf(filename, "img%d.bin", i);
        write_img(cur_im, filename);


        destroy_image(im);
        destroy_image(grad);
        free(best);
        free(path);
        im = cur_im;
    }
    destroy_image(im);*/
    
    //return 0;

//}