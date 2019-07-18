#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: resize n infile outfile\n");
        return 1;
    }

    //user input of n
    int n = atoi(argv[1]);

    //ensure n is less than or equal to 100, and n must be positive
    if (n < 0 || n > 100)
    {
        printf("Resize value must be positive and less than 100");
        return 1;
    }


    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf, bfNew;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    // new file header
    bfNew = bf;

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi, biNew;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    // new info header
    biNew = bi;

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    // write new outfile with *=n
    biNew.biWidth = bi.biWidth * n;
    biNew.biHeight = bi.biHeight * n;



    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    //determine new padding for scalines
    int newPadding = (4 - (biNew.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //new image size
    biNew.biSizeImage = ((sizeof(RGBTRIPLE) * biNew.biWidth) + newPadding) * abs (biNew.biHeight);
    bfNew.bfSize = biNew.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); //size of BITMAPFILEHEADER and BITMAPINFOHEADER is 54

    // write new outfile's BITMAPFILEHEADER
    fwrite(&bfNew, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write new outfile's BITMAPINFOHEADER
    fwrite(&biNew, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        //create a loop to print pixels by n factor

        for (int m = 0; m < n; m++)

        {

        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile, loop for width
            for (int r = 0; r < n; r++)
            {
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }

        }


        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add the new padding back
        for (int k = 0; k < newPadding; k++)
        {
            fputc(0x00, outptr);
        }
        // re reading row m
        if (m < n - 1)
        {
        long offset = bi.biWidth*sizeof(RGBTRIPLE)+padding;
        fseek(inptr, -offset, SEEK_CUR);
        }
      }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;

}