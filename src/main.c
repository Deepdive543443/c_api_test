#include "stdio.h"
#include "stdlib.h"

#include "detector/detector.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


int main(int argc, char** argv)
{
    int target_size = 416;

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
    {
        unsigned char *pixels_cpy = (unsigned char *) malloc(sizeof(unsigned char) * width * height * n);
        memcpy(pixels_cpy, pixels, sizeof(unsigned char) * width * height * n);
        Detector nanodet = create_nanodet(target_size, "../asset/nanodet-plus-m_416_int8.param", "../asset/nanodet-plus-m_416_int8.bin");
        BoxVec objects = nanodet.detect(pixels_cpy, width, height, &nanodet);
        printf("\n(Nanodet) Detected %ld items: \n", objects.num_item);
        for (int i=0; i < objects.num_item; i++)
        {
            BoxInfo box = objects.getItem(i, &objects);
            printf("%f %f %f %f %f %d\n", box.x1, box.x2, box.y1, box.y2, box.prob, box.label);
        }

        draw_boxxes(pixels_cpy, width, height, &objects);
        stbi_write_png("test_output.png", width, height, 3, pixels_cpy, width * 3);
        free(pixels_cpy);

        destroy_detector(&nanodet);
        objects.free(&objects);
    }
  
    /**
     * Create FastestDet
     * 
     */
    {
        unsigned char *pixels_cpy = (unsigned char *) malloc(sizeof(unsigned char) * width * height * n);
        memcpy(pixels_cpy, pixels, sizeof(unsigned char) * width * height * n);
        Detector fastestdet = create_fastestdet(352, "../asset/FastestDet.param", "../asset/FastestDet.bin");
        BoxVec objects = fastestdet.detect(pixels_cpy, width, height, &fastestdet);
        printf("\n(FastestDet) Detected %ld items: \n", objects.num_item);
        for (int i=0; i < objects.num_item; i++)
        {
            BoxInfo box = objects.getItem(i, &objects);
            printf("%f %f %f %f %f %d\n", box.x1, box.x2, box.y1, box.y2, box.prob, box.label);
        }

        draw_boxxes(pixels_cpy, width, height, &objects);
        stbi_write_png("test_output_fastest.png", width, height, 3, pixels_cpy, width * 3);
        free(pixels_cpy);

        destroy_detector(&fastestdet);
        objects.free(&objects);
    }


    printf("\nNANODET TEST\n\n");
}