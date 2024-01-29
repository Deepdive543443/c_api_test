#include "stdio.h"
#include "stdlib.h"
#include "c_api.h"

#include "detector/detector.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// #define STB_DS_IMPLEMENTATION
// #include "stb_ds.h"



int main(int argc, char** argv)
{
    // union 
    // {
    //     struct
    //     {
    //         __uint8_t r;
    //         __uint8_t g;
    //         __uint8_t b;
    //         __uint8_t a;
    //     };
    //     __uint32_t rgba;
    // } color;

    // color.r = 216;
    // color.g = 82;
    // color.b = 24;
    // color.a = 255;

    int target_size = 416;

    // Allocate pixel
    // unsigned char *pixel = malloc(sizeof(unsigned char) * 320 * 320 * 3);
    // memset(pixel, 255, 320 * 320 * 3); // White image 

    /**
     * Load image using stb image
     */
    int width, height, n;
    const char *file = "../asset/1.jpg";
    unsigned char *pixels = stbi_load(file, &width, &height, &n, 0);
    printf("%d %d %d\n", width, height, n);


    /**
     * Create nanodet
     * 
     */
    Detector nanodet = create_nanodet(target_size);
    BoxInfo *objects = NULL; // Dynamic array doesn't work for now, waiting for an implementation
    nanodet.detect(pixels, width, height, objects, &nanodet);
    free(pixels);

    printf("\nNANODET TEST\n\n");
}