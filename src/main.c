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
    Detector nanodet = create_nanodet(target_size);
    BoxVec objects = nanodet.detect(pixels, width, height, &nanodet);
    printf("Detected %ld items: \n", objects.num_item);
    for (int i=0; i < objects.num_item; i++)
    {
        BoxInfo box = objects.getItem(i, &objects);
        printf("%f %f %f %f %f %d\n", box.x1, box.x2, box.y1, box.y2, box.prob, box.label);
    }

    draw_boxxes(pixels, width, height, &objects);
    stbi_write_png("test_output.png", width, height, 3, pixels, width * 3);
    free(pixels);

    destroy_detector(&nanodet);

    printf("\nNANODET TEST\n\n");
}