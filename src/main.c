#include "stdio.h"
#include "stdlib.h"
#include "c_api.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "detector.h"


void print_mat(ncnn_mat_t *mat)
{
    printf("w: %d\nh: %d\nc: %d\nelesize: %d\ncstep: %d\n", ncnn_mat_get_w(mat), ncnn_mat_get_h(mat), ncnn_mat_get_c(mat), ncnn_mat_get_elemsize(mat), ncnn_mat_get_cstep(mat));
}

int main(int argc, char** argv)
{
    union 
    {
        struct
        {
            __uint8_t r;
            __uint8_t g;
            __uint8_t b;
            __uint8_t a;
        };
        __uint32_t rgba;
    } color;

    color.r = 216;
    color.g = 82;
    color.b = 24;
    color.a = 255;

    BoxInfo ahhhhh;

    int target_size = 416;

    const float mean_vals[] = {103.53f, 116.28f, 123.675f};
    const float norm_vals[] = {1.f / 57.375f, 1.f / 57.12f, 1.f / 58.395f};

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
    Detector nanodet = create_nanodet(416);
    nanodet.detect(pixels, width, height, &nanodet);
    free(pixels);

    printf("\nC API TEST\n");
}