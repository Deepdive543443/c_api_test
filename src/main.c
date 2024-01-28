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
    unsigned char *pixel = stbi_load(file, &width, &height, &n, 0);
    printf("%d %d %d\n", width, height, n);

    // Testing draw rectangle and 
    // ncnn_draw_rectangle_c3(pixel, width, height, 200, 200, 200, 200, (int) color.rgba, 3);
    // ncnn_draw_text_c3(pixel, width,  height, "Test label",200, 200, 7, (int) color.rgba);
    // stbi_write_png("test_output.png", width, height, n, pixel, width * 3);

    /**
     * Reshape and padding the matrix
     * to the times of 32
     */
    int w, h;
    float scale;
    if (width > height)
    {
        scale = (float) target_size / width;
        w = target_size;
        h = height * scale;
    }
    else
    {
        scale = (float) target_size / height;
        h = target_size;
        w = width * scale;
    }

    int wpad = (w + 31) / 32 * 32 - w;
    int hpad = (h + 31) / 32 * 32 - h;

    printf("%d %d %f\n", w, h, scale);
    printf("%d %d\n", w + wpad, h + hpad);

    /**
     * Create the NCNN matirx using pixels data
     */

    ncnn_allocator_t allocator = ncnn_allocator_create_pool_allocator();

    ncnn_mat_t mat = ncnn_mat_from_pixels_resize(pixel, NCNN_MAT_PIXEL_BGR, width, height, width * 3, w, h, allocator);
    printf("Input matrix: \n");
    print_mat(mat);

    ncnn_mat_t mat_pad = ncnn_mat_create();
    ncnn_option_t opt = ncnn_option_create();
    ncnn_copy_make_border(mat, mat_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, NCNN_BORDER_CONSTANT, 0.f, opt);
    ncnn_mat_destroy(mat);

    ncnn_mat_substract_mean_normalize(mat_pad, mean_vals, norm_vals);
    printf("\nPadded input matrix: \n");
    print_mat(mat_pad);
    

    /**
     * Create the network
     */
    ncnn_net_t net = ncnn_net_create();
    ncnn_net_load_param(net, "../asset/nanodet-plus-m_416_int8.param");
    ncnn_net_load_model(net, "../asset/nanodet-plus-m_416_int8.bin");
    // ncnn_net_load_param(net, "../FastestDet.param");
    // ncnn_net_load_model(net, "../FastestDet.bin");

    ncnn_extractor_t ex = ncnn_extractor_create(net);
    ncnn_extractor_input(ex, "data", mat_pad);

    /**
     * Extract output from 4 scales
     */

    const char* outputs[] = {"dis8", "cls8", "dis16", "cls16", "dis32", "cls32", "dis64", "cls64"};
    for (int i = 0;i < 8; i+=2)
    {
        ncnn_mat_t out_mat_dis;
        ncnn_mat_t out_mat_cls;
        ncnn_extractor_extract(ex, outputs[i], &out_mat_dis);
        ncnn_extractor_extract(ex, outputs[i+1], &out_mat_cls);   
        printf("\nOutput matrix(%s): \n", outputs[i]);
        print_mat(out_mat_dis);
        printf("Output matrix(%s): \n", outputs[i+1]);
        print_mat(out_mat_cls);

        ncnn_mat_destroy(out_mat_dis);
        ncnn_mat_destroy(out_mat_cls);
    }

    // Clean up
    ncnn_allocator_destroy(allocator);
    ncnn_option_destroy(opt);
    ncnn_extractor_destroy(ex);
    ncnn_net_destroy(net);

    free(pixel);

    printf("\nC API TEST\n");
}