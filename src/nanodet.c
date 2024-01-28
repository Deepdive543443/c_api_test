#include "detector.h"


static BoxInfo generate_proposals(ncnn_mat_t mat_dis, ncnn_mat_t mat_cls, int stride);


Detector create_nanodet(int input_size)
{
    Detector nanodet;
    nanodet.self = &nanodet;

    nanodet.net = ncnn_net_create();
    ncnn_net_load_param(nanodet.net, "../asset/nanodet-plus-m_416_int8.param");
    ncnn_net_load_model(nanodet.net, "../asset/nanodet-plus-m_416_int8.bin");

    nanodet.input_size = input_size;
    nanodet.mean_vals[0] = 103.53f;
    nanodet.mean_vals[1] = 116.28f;
    nanodet.mean_vals[2] = 123.675f;
    nanodet.norm_vals[0] = 1.f / 57.375f;
    nanodet.norm_vals[1] = 1.f / 57.12f;
    nanodet.norm_vals[2] = 1.f / 58.395f;

    nanodet.detect = &nanodet_detect;
    return nanodet;
}


BoxInfo nanodet_detect(unsigned char *pixels, int pixel_w, int pixel_h, void *self_ptr)
{
    BoxInfo output;
    Detector *self = (Detector *) self_ptr;
    
    int w, h;
    float scale;
    if (pixel_w > pixel_h)
    {
        scale = (float) self->input_size / pixel_w;
        w = self->input_size;
        h = pixel_h * scale;
    }
    else
    {
        scale = (float) self->input_size / pixel_h;
        h = self->input_size;
        w = pixel_w * scale;
    }

    int wpad = (w + 31) / 32 * 32 - w;
    int hpad = (h + 31) / 32 * 32 - h;

    printf("%d %d %f\n", w, h, scale);
    printf("%d %d\n", w + wpad, h + hpad);

    // /**
    //  * Create the NCNN matirx using pixels data
    //  */

    ncnn_allocator_t allocator = ncnn_allocator_create_pool_allocator();

    ncnn_mat_t mat = ncnn_mat_from_pixels_resize(pixels, NCNN_MAT_PIXEL_BGR, pixel_w, pixel_h, pixel_w * 3, w, h, allocator);
    printf("Input matrix: \n");
    print_mat(mat);

    ncnn_mat_t mat_pad = ncnn_mat_create();
    ncnn_option_t opt = ncnn_option_create();
    ncnn_copy_make_border(mat, mat_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, NCNN_BORDER_CONSTANT, 0.f, opt);
    ncnn_mat_destroy(mat);

    ncnn_mat_substract_mean_normalize(mat_pad, self->mean_vals, self->norm_vals);
    printf("\nPadded input matrix: \n");
    print_mat(mat_pad);
    

    /**
     * Create the network
     * This part suppose to be in create function
     */
    // ncnn_net_t net = ncnn_net_create();
    // ncnn_net_load_param(net, "../asset/nanodet-plus-m_416_int8.param");
    // ncnn_net_load_model(net, "../asset/nanodet-plus-m_416_int8.bin");
    // ncnn_net_load_param(net, "../FastestDet.param");
    // ncnn_net_load_model(net, "../FastestDet.bin");

    ncnn_extractor_t ex = ncnn_extractor_create(self->net);
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
    // ncnn_net_destroy(net);

    return output;
}