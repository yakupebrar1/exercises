// mario.c
// CS50x - Week 1, Problem Set 1
//
// Exercise: Mario (Less Comfortable)
// ------------------------------------
// Print a right-aligned half-pyramid of # symbols.
// The user provides the height (1–8).
//
// Expected output (height = 4):
//    #
//   ##
//  ###
// ####
//
// How to compile:  make mario
// How to run:      ./mario
// How to check:    check50 cs50/problems/2024/x/mario/less

#include <cs50.h>
#include <stdio.h>

int main(void)
{
    int height;
    do
    {
       height = get_int("Height: ");

    }
    while (height < 1 || height > 8);


    for (int row = 1; row <= height; row++)
    {
        for (int space = 0; space < height - row; space++)
        {
            printf(" ");
        }


        // TODO: Print (row) hashes using a for loop.
        for(int hash = 0; hash < row; hash++)
        {
            printf("#");
        }

        // Print a newline to end the row.
        printf("\n");
    }

    return 0;
}
