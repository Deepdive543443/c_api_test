#include "detector.h"

// #define STB_DS_IMPLEMENTATION
// #include "stb_ds.h"

static void generate_proposals(ncnn_mat_t dis_pred, ncnn_mat_t cls_pred, int stride, float prob_thresh, BoxInfo *objects)
{
    const int num_grid_x = ncnn_mat_get_w(cls_pred);
    const int num_grid_y = ncnn_mat_get_h(cls_pred);
    const int num_class = ncnn_mat_get_c(cls_pred);
    const int cstep_cls = ncnn_mat_get_cstep(cls_pred);

    const int reg_max_1 = ncnn_mat_get_w(dis_pred) / 4;
    const int hstep_dis = ncnn_mat_get_cstep(dis_pred);

    float *cls_data = (float *) ncnn_mat_get_data(cls_pred);
    float *dis_data = (float *) ncnn_mat_get_data(dis_pred);        

    // printf("num_grid_x, %d num_grid_y, %d num_class, %d cstep_cls, %d reg_max_1, %d hstep_dis, %d\n", num_grid_x, num_grid_y, num_class, cstep_cls, reg_max_1, hstep_dis);

    for (int i=0; i < num_grid_y; i++)
    {
        for(int j=0; j < num_grid_x; j++)
        {
            float *score_ptr = &cls_data[i * num_grid_y + j];
            float max_score = -FLT_MAX;
            int max_label = -1;

            for (int cls=0; cls < num_class; cls++)
            {
                if (score_ptr[cls * cstep_cls] > max_score)
                {
                    max_score = score_ptr[cls * cstep_cls];
                    max_label = cls;
                }

            }

            if (max_score >= prob_thresh)
            {
                float pred_ltrb[4];
                float *dis_ptr = &dis_data[(j * num_grid_y) + (i * hstep_dis)];
                for (int k=0; k < 4; k++)
                {
                    float dis = 0.f;
                    activation_function_softmax_inplace(dis_ptr, 8);
                    for (int reg=0; reg < reg_max_1; reg++)
                    {
                        dis += reg * dis_ptr[reg];
                    }
                    pred_ltrb[k] = dis * stride;
                    
                    dis_ptr += reg_max_1;
                }

                float x_center = j * stride;
                float y_center = i * stride;

                BoxInfo obj;
                obj.x1 = x_center - pred_ltrb[0];
                obj.y1 = y_center - pred_ltrb[1];
                obj.x2 = x_center + pred_ltrb[2];
                obj.y2 = y_center + pred_ltrb[3];
                obj.prob = max_score;
                obj.label = max_label;
                printf("%f %f %f %f %f %d\n", obj.x1, obj.x2, obj.y1, obj.y2, obj.prob, obj.label);

                // arrput(objects, obj);
            }
        }
    }

    // BoxInfo obj = objects[0];
    // printf("%f %f %f %f %f %d\n", objects[0].x1, objects[0].x2, objects[0].y1, objects[0].y2, objects[0].prob, objects[0].label);
}


Detector create_nanodet(int input_size)
{
    Detector nanodet;
    // nanodet.self = &nanodet;

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


BoxInfo *nanodet_detect(unsigned char *pixels, int pixel_w, int pixel_h, BoxInfo *objects, void *self_ptr)
{
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

    /**
     * Create the NCNN matirx using pixels data
     */

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
     * Create the extractor
     */

    ncnn_extractor_t ex = ncnn_extractor_create(self->net);
    ncnn_extractor_input(ex, "data", mat_pad);

    /**
     * Extract output from 4 scales
     */

    // BoxInfo *objects = malloc(sizeof(BoxInfo));
    const char* outputs[] = {"dis8", "cls8", "dis16", "cls16", "dis32", "cls32", "dis64", "cls64"};
    int strides[] = {8, 16, 32, 64};
    for (int i = 0;i < 4; i++)
    {
        ncnn_mat_t out_mat_dis;
        ncnn_mat_t out_mat_cls;
        ncnn_extractor_extract(ex, outputs[i * 2], &out_mat_dis);
        ncnn_extractor_extract(ex, outputs[i * 2 + 1], &out_mat_cls);   
        // printf("\nOutput matrix(%s): \n", outputs[i * 2]);
        // print_mat(out_mat_dis);
        // printf("Output matrix(%s): \n", outputs[i * 2 + 1]);
        // print_mat(out_mat_cls);

        generate_proposals(out_mat_dis, out_mat_cls, strides[i], 0.4, objects); // prob thresh 0.4

        ncnn_mat_destroy(out_mat_dis);
        ncnn_mat_destroy(out_mat_cls);
    }

    // Clean up
    ncnn_allocator_destroy(allocator);
    ncnn_option_destroy(opt);
    ncnn_extractor_destroy(ex);
    ncnn_mat_destroy(mat_pad);
    // ncnn_net_destroy(net);

    // BoxInfo obj = objects[3];
    // printf("%f %f %f %f %f %d\n", obj.x1, obj.x2, obj.y1, obj.y2, obj.prob, obj.label);

    return objects;
}