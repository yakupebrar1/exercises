// CS50x Week 4 — Filter (More): helpers.c
// ✏️  YOUR CODE GOES HERE
//
// Same as Filter-Less PLUS one harder function: edges()
// grayscale, reflect, blur — identical to Filter-Less
// edges — Sobel operator for edge detection (replaces sepia)

#include "helpers.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// TODO 1: GRAYSCALE  (same as Filter-Less)
// ---------------------------------------------------------------------------
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int r = image[i][j].rgbtRed;
            int g = image[i][j].rgbtGreen;
            int b = image[i][j].rgbtBlue;

            int avg = round((r + g + b) / 3.0);

            image[i][j].rgbtRed = avg;
            image[i][j].rgbtGreen = avg;
            image[i][j].rgbtBlue = avg;
        }
    }
}

// ---------------------------------------------------------------------------
// TODO 2: REFLECT  (same as Filter-Less)
// ---------------------------------------------------------------------------
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = temp;
        }
    }
}

// ---------------------------------------------------------------------------
// TODO 3: BLUR  (same as Filter-Less)
// ---------------------------------------------------------------------------
void blur(int height, int width, RGBTRIPLE image[height][width])
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE copy[height][width];
    memcpy(copy, image, sizeof(copy));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float sum_r = 0;
            float sum_g = 0;
            float sum_b = 0;
            int count = 0;

            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        sum_r += copy[ni][nj].rgbtRed;
                        sum_g += copy[ni][nj].rgbtGreen;
                        sum_b += copy[ni][nj].rgbtBlue;
                        count++;
                    }
                }
            }

            image[i][j].rgbtRed = round(sum_r / count);
            image[i][j].rgbtGreen = round(sum_g / count);
            image[i][j].rgbtBlue = round(sum_b / count);
        }
    }
}

// ---------------------------------------------------------------------------
// TODO 4: EDGES  ⭐⭐⭐⭐  (Filter-More exclusive — harder!)
// ---------------------------------------------------------------------------
// Detect edges using the Sobel operator.
//
// Background:
//   The Sobel operator uses two 3×3 kernels (Gx and Gy) to compute the
//   gradient of brightness in the horizontal and vertical directions.
//   Pixels with large gradients are "edges".
//
// Kernels:
//   Gx:          Gy:
//   -1  0  +1    -1 -2 -1
//   -2  0  +2     0  0  0
//   -1  0  +1    +1 +2 +1
//
// Algorithm (per channel, per pixel):
//   1. Make a copy of the image first.
//   2. For each pixel (i, j):
//      a. Treat out-of-bounds neighbours as black (R=G=B=0).
//      b. For each channel (R, G, B) separately:
//         - Compute Gx = sum of (Gx_kernel[di+1][dj+1] * neighbour_channel)
//         - Compute Gy = sum of (Gy_kernel[di+1][dj+1] * neighbour_channel)
//         - Final value = round(sqrt(Gx^2 + Gy^2))
//         - Cap at 255
//      c. Assign final R, G, B to image[i][j].
//
// HINT: Define the Gx and Gy kernels as 2D int arrays.
// HINT: Use (int) cast when reading copy channels to allow negative arithmetic.
// HINT: round(sqrt(Gx*Gx + Gy*Gy)) — use sqrt() and round() from <math.h>.
//
// Example (pixel surrounded by white pixels on left, black on right):
//   Gx for red channel will be large → edge detected → bright pixel output.
// ---------------------------------------------------------------------------
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    RGBTRIPLE copy[height][width];
    memcpy(copy, image, sizeof(copy));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int gx_r = 0, gy_r = 0;
            int gx_g = 0, gy_g = 0;
            int gx_b = 0, gy_b = 0;

            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        int kernelx = Gx[di + 1][dj + 1];
                        int kernely = Gy[di + 1][dj + 1];

                        gx_r += kernelx * copy[ni][nj].rgbtRed;
                        gy_r += kernely * copy[ni][nj].rgbtRed;

                        gx_g += kernelx * copy[ni][nj].rgbtGreen;
                        gy_g += kernely * copy[ni][nj].rgbtGreen;

                        gx_b += kernelx * copy[ni][nj].rgbtBlue;
                        gy_b += kernely * copy[ni][nj].rgbtBlue;
                    }
                }
            }

            int red = round(sqrt(gx_r * gx_r + gy_r * gy_r));
            int green = round(sqrt(gx_g * gx_g + gy_g * gy_g));
            int blue = round(sqrt(gx_b * gx_b + gy_b * gy_b));

            if (red > 255)
            {
                red = 255;
            }
            if (green > 255)
            {
                green = 255;
            }
            if (blue > 255)
            {
                blue = 255;
            }

            image[i][j].rgbtRed = red;
            image[i][j].rgbtGreen = green;
            image[i][j].rgbtBlue = blue;
        }
    }
}
