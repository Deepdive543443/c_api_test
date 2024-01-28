#pragma once
#include "c_api.h"

typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;
    int label;
    float prob;
} BoxInfo;

typedef BoxInfo (*Detect) (unsigned char *pixels, int pixel_w, int pixel_h, void *self_ptr);

typedef struct
{
    void *self;
    ncnn_net_t net;
    int input_size;
    float mean_vals[3];
    float norm_vals[3];
    Detect detect;
} Detector;



/**
 * Todo -- General function that share with all detector
 */

/**
 * Todo -- Nanodet's widget
 */

Detector create_nanodet(int input_size);
BoxInfo nanodet_detect(unsigned char *pixels, int input_w, int input_h, void *self_ptr);


/**
 * Todo -- FastestDet's widget
 */