#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK_SIZE 512
typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // jpeg recognition: 0xff, 0xd8, 0xff, 0xe(0, 1, 2, 3,4 ,5 , ..., d, f) --> the fourth byte's first four bits are [1 1 1 0] --> ( e )
    // look for JPEGs’ signatures. Each time you find a signature, you can open a new file for writing
    // and start filling that file with bytes from memory card, closing that file only once you encounter another signature.
    // rather than read memory card’s bytes one at a time, you can read 512 of them at a time into a buffer for efficiency’s sake.
    // Thanks to FAT, you can trust that JPEGs’ signatures will be “block-aligned.”
    // That is, you need only look for those signatures in a block’s first four bytes.
    // // Check first three bytes --> if (bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff)

    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }
    char *infile = argv[1];
    FILE *file = fopen(infile, "rb");
    if(file == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    BYTE buffer[BLOCK_SIZE];
    int a = 1;
    char filename[100] = "000.jpg";

    while ((fread(&buffer, sizeof(BYTE), BLOCK_SIZE, file)) != 0)
    {
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            FILE *newjpg = fopen(filename, "wb");
            fwrite(&buffer, sizeof(BYTE), BLOCK_SIZE, newjpg);
            while((fread(&buffer, sizeof(BYTE), BLOCK_SIZE, file)) != 0)
            {
                if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
                {
                    fclose(newjpg);
                    sprintf(filename, "%03i.jpg", a);
                    newjpg = fopen(filename, "wb");
                    fwrite(&buffer, sizeof(BYTE), BLOCK_SIZE, newjpg);
                    a++;

                }
                else
                {
                    fwrite(&buffer, sizeof(BYTE), BLOCK_SIZE, newjpg);
                }
            }
        }
    }
}