#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Validate command-line arguments
    if (argc != 2)
    {
        printf("Usage: ./recover IMAGE\n");
        return 1;
    }

    // Open memory card
    FILE *card = fopen(argv[1], "rb");
    if (card == NULL)
    {
        printf("Could not open %s.\n", argv[1]);
        return 1;
    }

    // Declare variables
    BYTE buffer[512];
    int count = 0;
    FILE *img = NULL;
    char filename[8];

    // Read card block by block
    while (fread(buffer, 1, 512, card) == 512)
    {
        // Check for JPEG signature
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            // Close previous file if open
            if (img != NULL)
            {
                fclose(img);
            }

            // Create new JPEG file
            sprintf(filename, "%03i.jpg", count);
            img = fopen(filename, "wb");
            count++;
        }

        // Write block if file open
        if (img != NULL)
        {
            fwrite(buffer, 1, 512, img);
        }
    }

    // Close files
    if (img != NULL)
    {
        fclose(img);
    }

    fclose(card);

    return 0;
}
