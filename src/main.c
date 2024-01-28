#include "stdio.h"
#include "stdlib.h"
#include "c_api.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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


    printf("%d %d %d %d \n", color.r, color.g, color.b, color.a);
    printf("%d \n", color.rgba);



    // Allocate pixel
    unsigned char *pixel = malloc(sizeof(unsigned char) * 320 * 320 * 3);
    // memset(pixel, 255, 320 * 320 * 3); // White image 
    ncnn_draw_rectangle_c3(pixel, 320, 320, 50, 50, 150, 150, (int) color.rgba, 2);
    ncnn_draw_text_c3(pixel, 320, 320, "Test label", 52, 52, 7, (int) color.rgba);
    stbi_write_png("test_output.png", 320, 320, 3, pixel, 320 * 3);

    // Create ncnn Matrix
    ncnn_mat_t *mat = ncnn_mat_from_pixels(pixel, NCNN_MAT_PIXEL_BGR, 320, 320, 1, ncnn_allocator_create_pool_allocator());
    // ncnn_mat_t *mat = ncnn_mat_create_3d(320, 320, 3, ncnn_allocator_create_pool_allocator());
    printf("Input matrix: \n");
    print_mat(mat);

    // printf("Input matrix: \n");
    // printf("w: %d\nh: %d\nc: %d\nelesize: %d\ncstep: %d\n", ncnn_mat_get_w(mat), ncnn_mat_get_h(mat), ncnn_mat_get_c(mat), ncnn_mat_get_elemsize(mat), ncnn_mat_get_cstep(mat));
    
    // Create network
    ncnn_net_t *net = ncnn_net_create();
    ncnn_net_load_param(net, "../asset/nanodet-plus-m_416_int8.param");
    ncnn_net_load_model(net, "../asset/nanodet-plus-m_416_int8.bin");
    // ncnn_net_load_param(net, "../FastestDet.param");
    // ncnn_net_load_model(net, "../FastestDet.bin");

    ncnn_extractor_t *ex = ncnn_extractor_create(net);

    ncnn_mat_t out_mat_dis8;
    ncnn_extractor_input(ex, "data", mat);
    ncnn_extractor_extract(ex, "dis8", &out_mat_dis8);
    printf("\nOutput matrix: \n");
    print_mat(out_mat_dis8);

    ncnn_mat_t out_mat_dis16;
    ncnn_extractor_input(ex, "data", mat);
    ncnn_extractor_extract(ex, "dis16", &out_mat_dis16);
    printf("\nOutput matrix: \n");
    print_mat(out_mat_dis16);

    // printf("\nOutput matrix: \n");
    // printf("w: %d\nh: %d\nc: %d\nelesize: %d\ncstep: %d\n", ncnn_mat_get_w(out_mat), ncnn_mat_get_h(out_mat), ncnn_mat_get_c(out_mat), ncnn_mat_get_elemsize(out_mat), ncnn_mat_get_cstep(out_mat));
    
    free(pixel);
    printf("C API TEST\n");
}